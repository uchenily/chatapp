#include "terminal.hpp"

/* At exit we'll try to fix the terminal to the initial conditions. */
void disableRawModeAtExit() { setRawMode(STDIN_FILENO, false); }

int setRawMode(int fd, bool enable) {
    /* We have a bit of global state (but local in scope) here.
     * This is needed to correctly set/undo raw mode. */
    static struct termios orig_termios; // Save original terminal status here.
    static bool atexit_registered
        = false; // Avoid registering atexit() many times.
    static bool rawmode_is_set = false; // True if raw mode was enabled.

    struct termios raw;

    /* If not enable, we just have to disable raw mode if it is
     * currently set. */
    if (not enable) {
        /* Don't even check the return value as it's too late. */
        if (rawmode_is_set && tcsetattr(fd, TCSAFLUSH, &orig_termios) != -1) {
            rawmode_is_set = false;
        }
        return 0;
    }

    /* Enable raw mode. */
    if (not(bool) isatty(fd)) {
        goto fatal;
    }
    if (not atexit_registered) {
        atexit(disableRawModeAtExit);
        atexit_registered = true;
    }
    if (tcgetattr(fd, &orig_termios) == -1) {
        goto fatal;
    }

    raw = orig_termios; /* modify the original mode */
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - do nothing. We want post processing enabled so that
     * \n will be automatically translated to \r\n. */
    // raw.c_oflag &= ...
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * but take signal chars (^Z,^C) enabled. */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    /* control chars - set return condition: min number of bytes and timer.
     * We want read to return every single byte, without timeout. */
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0; /* 1 byte, no timer */

    /* put terminal in raw mode after flushing */
    if (tcsetattr(fd, TCSAFLUSH, &raw) < 0) {
        goto fatal;
    }
    rawmode_is_set = true;
    return 0;

fatal:
    errno = ENOTTY;
    return -1;
}

// void terminalCleanCurrentLine() { write(fileno(stdout), "\e[2K", 4); }
void terminalCleanCurrentLine() { write(fileno(stdout), "\33[2K", 4); }

void terminalCursorAtLineStart() { write(fileno(stdout), "\r", 1); }

int inputBufferAppend(struct InputBuffer *ib, int c) {
    if (ib->len >= IB_MAX) {
        return IB_ERR; // No room.
    }

    ib->buf[ib->len] = (char) c;
    ib->len++;
    return IB_OK;
}

int inputBufferFeedChar(struct InputBuffer *ib, int c) {
    switch (c) {
        case '\n':
            break; // Ignored. We handle \r instead.
        case '\r':
            return IB_GOTLINE;
        case 127: // Backspace.
            if (ib->len > 0) {
                ib->len--;
                inputBufferHide(ib);
                inputBufferShow(ib);
            }
            break;
        default:
            if (inputBufferAppend(ib, c) == IB_OK) {
                write(fileno(stdout), ib->buf + ib->len - 1, 1);
            }
            break;
    }
    return IB_OK;
}

/* Hide the line the user is typing. */
void inputBufferHide(struct InputBuffer *ib) {
    (void) ib; // Not used var, but is conceptually part of the API.
    terminalCleanCurrentLine();
    terminalCursorAtLineStart();
}

/* Show again the current line. Usually called after InputBufferHide(). */
void inputBufferShow(struct InputBuffer *ib) {
    write(fileno(stdout), ib->buf, ib->len);
}

/* Reset the buffer to be empty. */
void inputBufferClear(struct InputBuffer *ib) {
    ib->len = 0;
    inputBufferHide(ib);
}
