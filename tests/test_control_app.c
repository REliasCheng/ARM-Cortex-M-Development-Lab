#include "control_app.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 0; } } while (0)
#define RUN(x) do { ++runs; if (!(x())) { ++fails; fprintf(stderr, "FAIL %s\n", #x); } } while (0)

static unsigned int runs, fails;

typedef struct {
    uint16_t samples[16];
    unsigned int sample_count;
    unsigned int sample_index;
    char frames[16][96];
    unsigned int frame_count;
    unsigned int starts;
    unsigned int stops;
    int fail_start;
    int fail_stop;
    int fail_read;
    int fail_write;
} Mock;

static int start(void *context)
{
    Mock *m = context;
    ++m->starts;
    return !m->fail_start;
}

static int stop(void *context)
{
    Mock *m = context;
    ++m->stops;
    return !m->fail_stop;
}

static int read_sample(void *context, uint16_t *sample)
{
    Mock *m = context;
    if (m->fail_read || m->sample_index >= m->sample_count) {
        return 0;
    }
    *sample = m->samples[m->sample_index++];
    return 1;
}

static int write_status(void *context, const char *data, size_t length)
{
    Mock *m = context;
    if (m->fail_write || m->frame_count >= 16U ||
        length >= sizeof m->frames[0]) {
        return 0;
    }
    memcpy(m->frames[m->frame_count], data, length);
    m->frames[m->frame_count][length] = '\0';
    ++m->frame_count;
    return 1;
}

static ControlPort port(Mock *m)
{
    ControlPort p = {m, start, stop, read_sample, write_status};
    return p;
}

static ControlConfig config(void)
{
    ControlConfig c = {10U, 50U, 1500U, 2500U};
    return c;
}

static int setup(ControlApp *app, Mock *m, uint32_t now)
{
    ControlPort p = port(m);
    ControlConfig c = config();
    CHECK(control_init(app, &p, &c, now) == CONTROL_OK);
    return 1;
}

static int test_normal_fifo_hysteresis(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int done;
    m.samples[0] = 2500U;
    m.samples[1] = 2000U;
    m.samples[2] = 1500U;
    m.sample_count = 3U;
    CHECK(setup(&a, &m, 0U));
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 20U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 30U) == CONTROL_OK);
    CHECK(control_poll(&a, 2U, &done) == CONTROL_OK && done == 2U);
    CHECK(m.frame_count == 2U);
    CHECK(control_poll(&a, 2U, &done) == CONTROL_OK && done == 1U);
    CHECK(strcmp(m.frames[0], "seq=1 tick=10 sample=2500 state=HIGH\n") == 0);
    CHECK(strcmp(m.frames[1], "seq=2 tick=20 sample=2000 state=HIGH\n") == 0);
    CHECK(strcmp(m.frames[2], "seq=3 tick=30 sample=1500 state=LOW\n") == 0);
    CHECK(a.sequence == 3U && a.level == CONTROL_LEVEL_LOW);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_IDLE && done == 0U);
    return 1;
}

static int test_initial_unknown_and_threshold_edges(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int done;
    m.samples[0] = 2000U; /* Mid-band cannot establish a level. */
    m.samples[1] = 2500U; /* Upper threshold is inclusive. */
    m.samples[2] = 1501U; /* Stay high within the hysteresis band. */
    m.samples[3] = 1500U; /* Lower threshold is inclusive. */
    m.sample_count = 4U;
    CHECK(setup(&a, &m, 0U));
    CHECK(a.level == CONTROL_LEVEL_UNKNOWN && a.sequence == 0U);
    CHECK(m.frame_count == 0U);
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 20U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 30U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 40U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_OK && done == 1U);
    CHECK(a.level == CONTROL_LEVEL_UNKNOWN && m.frame_count == 1U);
    CHECK(strcmp(m.frames[0], "seq=1 tick=10 sample=2000 state=UNKNOWN\n") == 0);
    CHECK(control_poll(&a, 3U, &done) == CONTROL_OK && done == 3U);
    CHECK(strcmp(m.frames[1], "seq=2 tick=20 sample=2500 state=HIGH\n") == 0);
    CHECK(strcmp(m.frames[2], "seq=3 tick=30 sample=1501 state=HIGH\n") == 0);
    CHECK(strcmp(m.frames[3], "seq=4 tick=40 sample=1500 state=LOW\n") == 0);
    CHECK(a.level == CONTROL_LEVEL_LOW && a.sequence == 4U);
    return 1;
}

static int test_recovery_discards_old_level_and_ticks(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int done;
    m.samples[0] = 3000U;
    m.samples[1] = 2000U;
    m.samples[2] = 1500U;
    m.sample_count = 3U;
    CHECK(setup(&a, &m, 0U));
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_OK && done == 1U);
    CHECK(a.level == CONTROL_LEVEL_HIGH);
    CHECK(control_post_tick_isr(&a, 20U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 21U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 22U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 23U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 24U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 25U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 26U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 27U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 28U) == CONTROL_QUEUE_FULL);
    CHECK(control_fault(&a) == CONTROL_FAULT_QUEUE_FULL);
    CHECK(control_recover(&a, 30U) == CONTROL_OK);
    CHECK(a.level == CONTROL_LEVEL_UNKNOWN && a.sequence == 1U);
    CHECK(control_poll(&a, 8U, &done) == CONTROL_IDLE && done == 0U);
    CHECK(m.frame_count == 1U && m.sample_index == 1U);
    CHECK(control_post_tick_isr(&a, 40U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_OK && done == 1U);
    CHECK(a.level == CONTROL_LEVEL_UNKNOWN);
    CHECK(strcmp(m.frames[1], "seq=2 tick=40 sample=2000 state=UNKNOWN\n") == 0);
    CHECK(control_post_tick_isr(&a, 50U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_OK && done == 1U);
    CHECK(a.level == CONTROL_LEVEL_LOW);
    CHECK(strcmp(m.frames[2], "seq=3 tick=50 sample=1500 state=LOW\n") == 0);
    return 1;
}

static int test_adc_extremes(void)
{
    ControlApp a = {0};
    Mock m = {0};
    ControlPort p = port(&m);
    ControlConfig c = {10U, 50U, 0U, CONTROL_ADC_MAX};
    unsigned int done;
    m.samples[0] = 0U;
    m.samples[1] = CONTROL_ADC_MAX;
    m.sample_count = 2U;
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 20U) == CONTROL_OK);
    CHECK(control_poll(&a, 2U, &done) == CONTROL_OK && done == 2U);
    CHECK(strcmp(m.frames[0], "seq=1 tick=10 sample=0 state=LOW\n") == 0);
    CHECK(strcmp(m.frames[1], "seq=2 tick=20 sample=4095 state=HIGH\n") == 0);
    CHECK(a.level == CONTROL_LEVEL_HIGH && a.sequence == 2U);
    return 1;
}

static int test_init_and_reinit(void)
{
    ControlApp a = {0};
    Mock m = {0};
    ControlPort p = port(&m);
    ControlConfig c = config();
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_OK);
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_INVALID_STATE);
    CHECK(m.starts == 1U);
    CHECK(control_shutdown(&a) == CONTROL_OK);
    CHECK(control_shutdown(&a) == CONTROL_OK && m.stops == 1U);
    CHECK(control_init(&a, &p, &c, 100U) == CONTROL_OK);
    CHECK(m.starts == 2U && control_state(&a) == CONTROL_RUNNING);
    return 1;
}

static int test_duplicate_and_early_tick(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int done;
    m.samples[0] = 1000U;
    m.sample_count = 1U;
    CHECK(setup(&a, &m, 0U));
    CHECK(control_post_tick_isr(&a, 0U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 9U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_poll(&a, 4U, &done) == CONTROL_OK && done == 1U);
    CHECK(m.sample_index == 1U && m.frame_count == 1U);
    return 1;
}

static int test_tick_gap_and_wrap(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int done;
    CHECK(setup(&a, &m, 0U));
    CHECK(control_post_tick_isr(&a, 51U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_FAULTED);
    CHECK(control_fault(&a) == CONTROL_FAULT_TICK_GAP);
    CHECK(m.sample_index == 0U && m.frame_count == 0U);
    CHECK(control_recover(&a, UINT32_MAX - 4U) == CONTROL_OK);
    m.samples[0] = 100U;
    m.sample_count = 1U;
    CHECK(control_post_tick_isr(&a, 5U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_OK && done == 1U);
    CHECK(control_fault(&a) == CONTROL_FAULT_NONE);
    return 1;
}

static int test_queue_full_latches_fault(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int i, done;
    CHECK(setup(&a, &m, 0U));
    for (i = 0U; i < CONTROL_TICK_CAPACITY; ++i) {
        CHECK(control_post_tick_isr(&a, 10U * (i + 1U)) == CONTROL_OK);
    }
    CHECK(control_post_tick_isr(&a, 90U) == CONTROL_QUEUE_FULL);
    CHECK(control_state(&a) == CONTROL_FAULT);
    CHECK(control_fault(&a) == CONTROL_FAULT_QUEUE_FULL);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_INVALID_STATE);
    CHECK(control_post_tick_isr(&a, 100U) == CONTROL_INVALID_STATE);
    CHECK(m.frame_count == 0U);
    m.samples[0] = 1000U;
    m.sample_count = 1U;
    CHECK(control_recover(&a, 90U) == CONTROL_OK);
    CHECK(control_poll(&a, 8U, &done) == CONTROL_IDLE && done == 0U);
    CHECK(control_post_tick_isr(&a, 100U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_OK && done == 1U);
    CHECK(m.frame_count == 1U);
    return 1;
}

static int test_bad_input(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int done;
    m.samples[0] = 4096U;
    m.sample_count = 1U;
    CHECK(setup(&a, &m, 0U));
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_FAULTED);
    CHECK(control_fault(&a) == CONTROL_FAULT_INPUT);
    CHECK(m.frame_count == 0U && a.sequence == 0U);
    return 1;
}

static int test_read_failure_and_recovery(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int done;
    CHECK(setup(&a, &m, 0U));
    m.fail_read = 1;
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_IO_ERROR);
    CHECK(control_fault(&a) == CONTROL_FAULT_READ && m.frame_count == 0U);
    CHECK(control_recover(&a, 10U) == CONTROL_OK);
    CHECK(m.stops == 1U && m.starts == 2U);
    m.fail_read = 0;
    m.samples[0] = 3000U;
    m.sample_count = 1U;
    CHECK(control_post_tick_isr(&a, 20U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_OK && done == 1U);
    CHECK(m.frame_count == 1U && a.level == CONTROL_LEVEL_HIGH);
    return 1;
}

static int test_write_failure_preserves_history(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int done;
    m.samples[0] = 3000U;
    m.sample_count = 1U;
    CHECK(setup(&a, &m, 0U));
    m.fail_write = 1;
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_IO_ERROR);
    CHECK(control_fault(&a) == CONTROL_FAULT_WRITE);
    CHECK(a.sequence == 0U && a.level == CONTROL_LEVEL_UNKNOWN &&
          a.last_sample_tick == 0U);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_INVALID_STATE);
    CHECK(m.frame_count == 0U);
    return 1;
}

static int test_start_stop_failures(void)
{
    ControlApp a = {0};
    Mock m = {0};
    ControlPort p = port(&m);
    ControlConfig c = config();
    m.fail_start = 1;
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_IO_ERROR);
    CHECK(control_fault(&a) == CONTROL_FAULT_START);
    m.fail_stop = 1;
    CHECK(control_recover(&a, 0U) == CONTROL_IO_ERROR);
    CHECK(control_fault(&a) == CONTROL_FAULT_STOP && m.starts == 1U);
    m.fail_stop = 0;
    m.fail_start = 0;
    CHECK(control_recover(&a, 0U) == CONTROL_OK);
    m.fail_read = 1;
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    {
        unsigned int done;
        CHECK(control_poll(&a, 1U, &done) == CONTROL_IO_ERROR);
    }
    m.fail_start = 1;
    CHECK(control_recover(&a, 10U) == CONTROL_IO_ERROR);
    CHECK(control_fault(&a) == CONTROL_FAULT_START);
    m.fail_start = 0;
    m.fail_read = 0;
    CHECK(control_recover(&a, 10U) == CONTROL_OK);
    m.fail_stop = 1;
    CHECK(control_shutdown(&a) == CONTROL_IO_ERROR);
    CHECK(control_state(&a) == CONTROL_FAULT);
    m.fail_stop = 0;
    CHECK(control_shutdown(&a) == CONTROL_OK);
    return 1;
}

static int test_sequence_overflow(void)
{
    ControlApp a = {0};
    Mock m = {0};
    unsigned int done;
    m.samples[0] = 100U;
    m.sample_count = 1U;
    CHECK(setup(&a, &m, 0U));
    a.sequence = UINT32_MAX;
    CHECK(control_post_tick_isr(&a, 10U) == CONTROL_OK);
    CHECK(control_poll(&a, 1U, &done) == CONTROL_FAULTED);
    CHECK(control_fault(&a) == CONTROL_FAULT_SEQUENCE && m.frame_count == 0U);
    CHECK(control_recover(&a, 10U) == CONTROL_INVALID_STATE);
    CHECK(control_shutdown(&a) == CONTROL_OK);
    CHECK(control_state(&a) == CONTROL_STOPPED);
    return 1;
}

static int test_invalid_arguments(void)
{
    ControlApp a = {0};
    Mock m = {0};
    ControlPort p = port(&m);
    ControlConfig c = config();
    unsigned int done;
    CHECK(control_init(NULL, &p, &c, 0U) == CONTROL_INVALID_ARGUMENT);
    c.sample_period_ms = 0U;
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_INVALID_ARGUMENT);
    c = config();
    c.high_threshold = 4096U;
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_INVALID_ARGUMENT);
    c = config();
    c.high_threshold = c.low_threshold;
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_INVALID_ARGUMENT);
    c = config();
    c.max_tick_gap_ms = c.sample_period_ms - 1U;
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_INVALID_ARGUMENT);
    c = config();
    c.sample_period_ms = (uint32_t)INT32_MAX + 1U;
    c.max_tick_gap_ms = c.sample_period_ms;
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_INVALID_ARGUMENT);
    c = config();
    p.write_status = NULL;
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_INVALID_ARGUMENT);
    p = port(&m);
    CHECK(control_init(&a, &p, &c, 0U) == CONTROL_OK);
    CHECK(control_poll(&a, 0U, &done) == CONTROL_INVALID_ARGUMENT);
    CHECK(control_poll(&a, 1U, NULL) == CONTROL_INVALID_ARGUMENT);
    CHECK(control_recover(&a, 0U) == CONTROL_INVALID_STATE);
    return 1;
}

int main(void)
{
    RUN(test_normal_fifo_hysteresis);
    RUN(test_initial_unknown_and_threshold_edges);
    RUN(test_recovery_discards_old_level_and_ticks);
    RUN(test_adc_extremes);
    RUN(test_init_and_reinit);
    RUN(test_duplicate_and_early_tick);
    RUN(test_tick_gap_and_wrap);
    RUN(test_queue_full_latches_fault);
    RUN(test_bad_input);
    RUN(test_read_failure_and_recovery);
    RUN(test_write_failure_preserves_history);
    RUN(test_start_stop_failures);
    RUN(test_sequence_overflow);
    RUN(test_invalid_arguments);
    printf("control_app: %u tests, %u failures\n", runs, fails);
    return fails == 0U ? 0 : 1;
}
