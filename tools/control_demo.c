#include "control_app.h"

#include <stdio.h>

typedef struct {
    uint16_t samples[3];
    unsigned int next;
} DemoBoard;

static int board_start(void *context)
{
    (void)context;
    return 1;
}

static int board_stop(void *context)
{
    (void)context;
    return 1;
}

static int board_read(void *context, uint16_t *sample)
{
    DemoBoard *board = context;
    if (board->next >= 3U) {
        return 0;
    }
    *sample = board->samples[board->next++];
    return 1;
}

static int board_write(void *context, const char *frame, size_t length)
{
    (void)context;
    return fwrite(frame, 1U, length, stdout) == length;
}

int main(void)
{
    DemoBoard board = {{2600U, 2000U, 1400U}, 0U};
    ControlPort port = {&board, board_start, board_stop, board_read,
                        board_write};
    ControlConfig config = {10U, 50U, 1500U, 2500U};
    ControlApp app = {0};
    unsigned int processed;
    if (control_init(&app, &port, &config, 0U) != CONTROL_OK) {
        return 1;
    }
    if (control_post_tick_isr(&app, 10U) != CONTROL_OK ||
        control_post_tick_isr(&app, 20U) != CONTROL_OK ||
        control_post_tick_isr(&app, 30U) != CONTROL_OK ||
        control_poll(&app, 3U, &processed) != CONTROL_OK ||
        processed != 3U || control_shutdown(&app) != CONTROL_OK) {
        return 2;
    }
    return 0;
}
