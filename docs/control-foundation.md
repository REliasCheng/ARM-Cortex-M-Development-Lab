# Timer-to-sample control foundation

This repository now includes an **original portable C11 Host control core**.
It models a Cortex-M application boundary but contains no GD32/STM32 register
access, startup code, real ADC driver, UART driver, or board pin assignment.

```text
board timer callback -> control_post_tick_isr() -> bounded SPSC tick ring
                                              -> main-loop control_poll()
                                              -> board read_sample()
                                              -> hysteresis state decision
                                              -> board write_status()
```

The timer-side function only enqueues a 32-bit millisecond timestamp. Sampling,
formatting, and output occur in the main loop. A `ControlPort` supplies
`start`, `stop`, `read_sample`, and `write_status` callbacks; it is a contract,
not a hardware driver. The caller owns a zero-initialized `ControlApp` and
keeps it alive while either context uses it. Only one producer and one consumer
may access the ring. Initialization, recovery, and shutdown require the timer
producer to be quiesced.

The config defines a sampling period, maximum accepted tick gap, and low/high
thresholds. The low threshold must be strictly below the high threshold. The
input model is a 12-bit value in `0..4095`; this does not
assert a particular ADC channel or voltage. The state changes to `HIGH` at
or above the high threshold and back to `LOW` at or below the low threshold.
Values between thresholds retain the previous state. At initialization and
after recovery that state is `UNKNOWN` until a sample crosses either
threshold; a mid-band reading is reported as `UNKNOWN` rather than claiming
an unobserved `LOW` or `HIGH`. Initialization emits no status frame. Every accepted sample
produces one bounded text frame, for example:

```text
seq=1 tick=10 sample=2600 state=HIGH
```

The sequence increases only after the output callback reports that the
complete frame was accepted. A callback failure may have partially sent a
frame; the core latches a fault and does not automatically retry, so external
observers must treat the last attempted output as uncertain.

## State and fault contract

`control_init` starts the platform callbacks; repeating it while running or
faulted is rejected. `control_post_tick_isr` returns queue-full and latches a
fault on the ninth unconsumed event. `control_poll` bounds work by the number
of **dequeued** events, not only successful samples. Duplicate/early ticks
are ignored. A gap greater than `max_tick_gap_ms` (including a backwards
timestamp interpreted as a large unsigned gap), invalid sample, callback
error, or sequence exhaustion latches `CONTROL_FAULT`. A valid 32-bit timer
wrap is supported when the real elapsed gap stays within the configured
half-range bound.

Once faulted, new tick posts and main-loop processing are rejected.
`control_recover` is explicit: with timer delivery quiesced, it calls `stop`
then `start`, clears the queue and sets the hysteresis state to `UNKNOWN`, and establishes a new
time baseline. It preserves the output sequence, so the first accepted frame
after recovery has the next sequence number. A failing stop or start leaves the software fault latched.
`control_shutdown` also reports a failed stop instead of claiming the board
is inactive. Neither operation is a mechanical or electrical safety action.
Sequence exhaustion requires shutdown and a new `control_init` session;
recovery will not wrap the sequence number.

The ring uses C11 atomic indices to express a single-producer,
single-consumer software contract. Before placing this function in a real
ISR, a target build and disassembly/runtime review must confirm that the
selected compiler emits suitable bounded interrupt-safe atomic operations;
otherwise the board adapter needs an explicit critical-section strategy.
The Host tests do not prove interrupt timing or physical I/O behavior.

## Reproduce on a PC

```sh
gcc -std=c11 -Wall -Wextra -Werror -pedantic -I include \
  src/control_app.c tests/test_control_app.c -o control_test
./control_test
gcc -std=c11 -Wall -Wextra -Werror -pedantic -I include \
  src/control_app.c tools/control_demo.c -o control_demo
./control_demo
```

The demo injects ticks at 10/20/30 ms and samples 2600/2000/1400. It prints
`HIGH`, `HIGH`, `LOW` in sequence. The tests also cover initialization,
repeated calls, queue exhaustion, wraparound, timing gaps, invalid values,
inclusive threshold edges, initial and recovered `UNKNOWN` output, ADC extremes,
callback failures, shutdown, and explicit recovery. GitHub Actions runs the
same source with GCC, Clang, ASan, and UBSan.
