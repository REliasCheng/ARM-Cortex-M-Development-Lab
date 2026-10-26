#include "control_app.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#define CONTROL_SIGNATURE UINT32_C(0x4354524C)

static int valid_config(const ControlConfig *c)
{
    return c != NULL && c->sample_period_ms > 0U &&
           c->sample_period_ms <= (uint32_t)INT32_MAX &&
           c->max_tick_gap_ms >= c->sample_period_ms &&
           c->max_tick_gap_ms <= (uint32_t)INT32_MAX &&
           c->low_threshold < c->high_threshold &&
           c->high_threshold <= CONTROL_ADC_MAX;
}

static void latch_fault(ControlApp *app, ControlFault fault)
{
    atomic_store_explicit(&app->fault, (unsigned int)fault, memory_order_relaxed);
    atomic_store_explicit(&app->state, CONTROL_FAULT, memory_order_release);
}

ControlState control_state(const ControlApp *app)
{
    if (app == NULL || app->signature != CONTROL_SIGNATURE) {
        return CONTROL_NEW;
    }
    return (ControlState)atomic_load_explicit(&app->state, memory_order_acquire);
}

ControlFault control_fault(const ControlApp *app)
{
    if (app == NULL || app->signature != CONTROL_SIGNATURE) {
        return CONTROL_FAULT_NONE;
    }
    return (ControlFault)atomic_load_explicit(&app->fault, memory_order_acquire);
}

ControlResult control_init(ControlApp *app, const ControlPort *port,
                           const ControlConfig *config, uint32_t now_ms)
{
    if (app == NULL || port == NULL || port->context == NULL ||
        port->start == NULL || port->stop == NULL ||
        port->read_sample == NULL || port->write_status == NULL ||
        !valid_config(config)) {
        return CONTROL_INVALID_ARGUMENT;
    }
    if (app->signature == CONTROL_SIGNATURE &&
        control_state(app) != CONTROL_STOPPED) {
        return CONTROL_INVALID_STATE;
    }
    memset(app, 0, sizeof *app);
    app->signature = CONTROL_SIGNATURE;
    app->port = *port;
    app->config = *config;
    app->last_sample_tick = now_ms;
    app->level = CONTROL_LEVEL_UNKNOWN;
    atomic_init(&app->head, 0U);
    atomic_init(&app->tail, 0U);
    atomic_init(&app->fault, CONTROL_FAULT_NONE);
    atomic_init(&app->state, CONTROL_NEW);
    if (!app->port.start(app->port.context)) {
        latch_fault(app, CONTROL_FAULT_START);
        return CONTROL_IO_ERROR;
    }
    atomic_store_explicit(&app->state, CONTROL_RUNNING, memory_order_release);
    return CONTROL_OK;
}

ControlResult control_post_tick_isr(ControlApp *app, uint32_t now_ms)
{
    unsigned int head;
    unsigned int tail;
    if (app == NULL || app->signature != CONTROL_SIGNATURE) {
        return CONTROL_INVALID_ARGUMENT;
    }
    if (control_state(app) != CONTROL_RUNNING) {
        return CONTROL_INVALID_STATE;
    }
    tail = atomic_load_explicit(&app->tail, memory_order_relaxed);
    head = atomic_load_explicit(&app->head, memory_order_acquire);
    if (tail - head >= CONTROL_TICK_CAPACITY) {
        latch_fault(app, CONTROL_FAULT_QUEUE_FULL);
        return CONTROL_QUEUE_FULL;
    }
    app->ticks[tail % CONTROL_TICK_CAPACITY] = now_ms;
    atomic_store_explicit(&app->tail, tail + 1U, memory_order_release);
    return CONTROL_OK;
}

ControlResult control_poll(ControlApp *app, unsigned int budget,
                           unsigned int *processed)
{
    unsigned int done = 0U;
    unsigned int visited = 0U;
    if (app == NULL || processed == NULL || budget == 0U ||
        app->signature != CONTROL_SIGNATURE) {
        return CONTROL_INVALID_ARGUMENT;
    }
    *processed = 0U;
    if (control_state(app) != CONTROL_RUNNING) {
        return CONTROL_INVALID_STATE;
    }
    while (visited < budget) {
        unsigned int head = atomic_load_explicit(&app->head, memory_order_relaxed);
        unsigned int tail = atomic_load_explicit(&app->tail, memory_order_acquire);
        uint32_t tick;
        uint32_t gap;
        uint16_t sample;
        ControlLevel candidate_level;
        char frame[96];
        int length;
        if (head == tail) {
            break;
        }
        tick = app->ticks[head % CONTROL_TICK_CAPACITY];
        atomic_store_explicit(&app->head, head + 1U, memory_order_release);
        ++visited;
        gap = tick - app->last_sample_tick; /* Unsigned wrap is intentional. */
        if (gap == 0U || gap < app->config.sample_period_ms) {
            continue; /* Duplicate or early event. */
        }
        if (gap > app->config.max_tick_gap_ms) {
            latch_fault(app, CONTROL_FAULT_TICK_GAP);
            return CONTROL_FAULTED;
        }
        if (!app->port.read_sample(app->port.context, &sample)) {
            latch_fault(app, CONTROL_FAULT_READ);
            return CONTROL_IO_ERROR;
        }
        if (sample > CONTROL_ADC_MAX) {
            latch_fault(app, CONTROL_FAULT_INPUT);
            return CONTROL_FAULTED;
        }
        if (app->sequence == UINT32_MAX) {
            latch_fault(app, CONTROL_FAULT_SEQUENCE);
            return CONTROL_FAULTED;
        }
        candidate_level = app->level;
        if (sample >= app->config.high_threshold) {
            candidate_level = CONTROL_LEVEL_HIGH;
        } else if (sample <= app->config.low_threshold) {
            candidate_level = CONTROL_LEVEL_LOW;
        }
        length = snprintf(frame, sizeof frame,
                          "seq=%lu tick=%lu sample=%u state=%s\n",
                          (unsigned long)(app->sequence + 1U),
                          (unsigned long)tick, (unsigned int)sample,
                          candidate_level == CONTROL_LEVEL_HIGH ? "HIGH" :
                          candidate_level == CONTROL_LEVEL_LOW ? "LOW" : "UNKNOWN");
        if (length < 0 || (size_t)length >= sizeof frame) {
            latch_fault(app, CONTROL_FAULT_WRITE);
            return CONTROL_FAULTED;
        }
        if (control_state(app) != CONTROL_RUNNING) {
            return CONTROL_FAULTED;
        }
        if (!app->port.write_status(app->port.context, frame, (size_t)length)) {
            latch_fault(app, CONTROL_FAULT_WRITE);
            return CONTROL_IO_ERROR;
        }
        app->level = candidate_level;
        app->last_sample_tick = tick;
        ++app->sequence;
        ++done;
        *processed = done;
        if (control_state(app) != CONTROL_RUNNING) {
            return CONTROL_FAULTED;
        }
    }
    return done == 0U ? CONTROL_IDLE : CONTROL_OK;
}

ControlResult control_recover(ControlApp *app, uint32_t now_ms)
{
    if (app == NULL || app->signature != CONTROL_SIGNATURE) {
        return CONTROL_INVALID_ARGUMENT;
    }
    if (control_state(app) != CONTROL_FAULT) {
        return CONTROL_INVALID_STATE;
    }
    if (control_fault(app) == CONTROL_FAULT_SEQUENCE) {
        return CONTROL_INVALID_STATE; /* New session required; no sequence wrap. */
    }
    if (!app->port.stop(app->port.context)) {
        latch_fault(app, CONTROL_FAULT_STOP);
        return CONTROL_IO_ERROR;
    }
    if (!app->port.start(app->port.context)) {
        latch_fault(app, CONTROL_FAULT_START);
        return CONTROL_IO_ERROR;
    }
    atomic_store_explicit(&app->head, 0U, memory_order_relaxed);
    atomic_store_explicit(&app->tail, 0U, memory_order_relaxed);
    app->last_sample_tick = now_ms;
    app->level = CONTROL_LEVEL_UNKNOWN;
    atomic_store_explicit(&app->fault, CONTROL_FAULT_NONE, memory_order_relaxed);
    atomic_store_explicit(&app->state, CONTROL_RUNNING, memory_order_release);
    return CONTROL_OK;
}

ControlResult control_shutdown(ControlApp *app)
{
    if (app == NULL || app->signature != CONTROL_SIGNATURE) {
        return CONTROL_INVALID_ARGUMENT;
    }
    if (control_state(app) == CONTROL_STOPPED) {
        return CONTROL_OK;
    }
    if (control_state(app) != CONTROL_RUNNING &&
        control_state(app) != CONTROL_FAULT) {
        return CONTROL_INVALID_STATE;
    }
    if (!app->port.stop(app->port.context)) {
        latch_fault(app, CONTROL_FAULT_STOP);
        return CONTROL_IO_ERROR;
    }
    atomic_store_explicit(&app->state, CONTROL_STOPPED, memory_order_release);
    return CONTROL_OK;
}
