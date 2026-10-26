#ifndef CONTROL_APP_H
#define CONTROL_APP_H

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

/* One timer-event producer and one main-loop consumer. No dynamic storage. */
#define CONTROL_TICK_CAPACITY 8U
#define CONTROL_ADC_MAX UINT16_C(4095)

typedef enum {
    CONTROL_NEW = 0,
    CONTROL_RUNNING,
    CONTROL_FAULT,
    CONTROL_STOPPED
} ControlState;

typedef enum {
    CONTROL_FAULT_NONE = 0,
    CONTROL_FAULT_START,
    CONTROL_FAULT_STOP,
    CONTROL_FAULT_QUEUE_FULL,
    CONTROL_FAULT_TICK_GAP,
    CONTROL_FAULT_INPUT,
    CONTROL_FAULT_READ,
    CONTROL_FAULT_WRITE,
    CONTROL_FAULT_SEQUENCE
} ControlFault;

typedef enum {
    CONTROL_LEVEL_LOW = 0,
    CONTROL_LEVEL_HIGH,
    CONTROL_LEVEL_UNKNOWN
} ControlLevel;

typedef enum {
    CONTROL_OK = 0,
    CONTROL_IDLE,
    CONTROL_INVALID_ARGUMENT,
    CONTROL_INVALID_STATE,
    CONTROL_QUEUE_FULL,
    CONTROL_IO_ERROR,
    CONTROL_FAULTED
} ControlResult;

typedef struct {
    void *context;
    int (*start)(void *context);
    int (*stop)(void *context);
    int (*read_sample)(void *context, uint16_t *sample);
    /* Return 1 only when the complete frame was accepted by the adapter. */
    int (*write_status)(void *context, const char *data, size_t length);
} ControlPort;

typedef struct {
    uint32_t sample_period_ms;
    uint32_t max_tick_gap_ms;
    uint16_t low_threshold;
    uint16_t high_threshold;
} ControlConfig;

typedef struct {
    uint32_t signature; /* Caller zero-initializes this object before first init. */
    ControlPort port;
    ControlConfig config;
    atomic_uint state;
    atomic_uint fault;
    atomic_uint head;
    atomic_uint tail;
    uint32_t ticks[CONTROL_TICK_CAPACITY];
    uint32_t last_sample_tick;
    uint32_t sequence;
    ControlLevel level;
} ControlApp;

/* Call init/start, recover and shutdown only with the timer producer quiesced. */
ControlResult control_init(ControlApp *app, const ControlPort *port,
                           const ControlConfig *config, uint32_t now_ms);
/* ISR boundary: stores one timestamp, performs no I/O or application work. */
ControlResult control_post_tick_isr(ControlApp *app, uint32_t now_ms);
/* Main-loop boundary: processes up to budget events, in FIFO order. */
ControlResult control_poll(ControlApp *app, unsigned int budget,
                           unsigned int *processed);
/* Explicit recovery: stop, restart and discard queued events on success. */
ControlResult control_recover(ControlApp *app, uint32_t now_ms);
ControlResult control_shutdown(ControlApp *app);
ControlState control_state(const ControlApp *app);
ControlFault control_fault(const ControlApp *app);

#endif
