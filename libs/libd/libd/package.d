module libd;

import core.vararg;

__gshared uint stdin;
__gshared uint stdout;

enum TTY_IOCTL {
    CLEAR = 1 << 8,
    READ_MODE_LINE_ON = (2 << 8),
    READ_MODE_ECHO_ON = (3 << 8),
    READ_MODE_LINE_OFF = (4 << 8),
    READ_MODE_ECHO_OFF = (5 << 8),

    READ = 0x0,
    WRITE = 0x1,
}

@nogc nothrow @system extern (C) {
    void stdio_init();
}

@nogc nothrow @system extern (C) pure {
    void uprintf(scope const char* format, ...);
    void scanf(scope const char* format, ...);

    void uvprintf(scope const char* format, va_list argptr);
    void vscanf(scope const char* format, va_list argptr);

    void sprintf(scope const char* buff, scope const char* format, ...);

    uint fsyscall(uint id, uint arg1, uint arg2, uint arg3);
}

nothrow @system extern (C) void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    uvprintf(format, args);
    va_end(args);
}

pragma(inline, true) @nogc nothrow {
    uint syscall_open(uint path, uint mode) {
        return fsyscall(0x5, path, mode, 0);
    }

    uint syscall_close(uint fd) {
        return fsyscall(0x6, fd, 0, 0);
    }

    uint syscall_read(uint fd, uint buff, uint size) {
        return fsyscall(0x7, fd, buff, size);
    }

    uint syscall_write(uint fd, uint buff, uint size) {
        return fsyscall(0x8, fd, buff, size);
    }

    uint syscall_ioctl(uint fd, uint cmd, uint subcmd) {
        return fsyscall(0x9, fd, cmd, subcmd);
    }
}
