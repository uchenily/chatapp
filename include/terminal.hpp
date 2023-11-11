#pragma once
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/select.h>

/* ============================================================================
 * Low level terminal handling.
 * ========================================================================== */

void disableRawModeAtExit();

/* Raw mode: 1960 magic shit. */
int setRawMode(int fd, bool enable);

/* ============================================================================
 * Mininal line editing.
 * ========================================================================== */

void terminalCleanCurrentLine();

void terminalCursorAtLineStart();

#define IB_MAX 128
struct InputBuffer {
    char buf[IB_MAX]; // Buffer holding the data.
    int len;          // Current length.
};

/* inputBuffer*() return values: */
#define IB_ERR 0     // Sorry, unable to comply.
#define IB_OK 1      // Ok, got the new char, did the operation, ...
#define IB_GOTLINE 2 // Hey, now there is a well formed line to read.

/* Append the specified character to the buffer. */
int inputBufferAppend(struct InputBuffer *ib, int c);

void inputBufferHide(struct InputBuffer *ib);
void inputBufferShow(struct InputBuffer *ib);

/* Process every new keystroke arriving from the keyboard. As a side effect
 * the input buffer state is modified in order to reflect the current line
 * the user is typing, so that reading the input buffer 'buf' for 'len'
 * bytes will contain it. */
int inputBufferFeedChar(struct InputBuffer *ib, int c);

/* Hide the line the user is typing. */
void inputBufferHide(struct InputBuffer *ib);

/* Show again the current line. Usually called after InputBufferHide(). */
void inputBufferShow(struct InputBuffer *ib);

/* Reset the buffer to be empty. */
void inputBufferClear(struct InputBuffer *ib);
