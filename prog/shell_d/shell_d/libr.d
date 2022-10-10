module prog.shell_d.shell_d.libr;

import core.vararg;

__gshared uint stdin;
__gshared uint stdout;

@nogc nothrow @system extern (C) void stdio_init();
@nogc nothrow @system extern (C) void uprintf(scope const char* format, ...);
@nogc nothrow @system extern (C) void scanf(scope const char* format, ...);

@nogc nothrow @system extern (C) void uvprintf(scope const char* format, va_list argptr);
@nogc nothrow @system extern (C) void vscanf(scope const char* format, va_list argptr);

@nogc nothrow @system extern (C) void sprintf(scope const char* buff, scope const char* format, ...);

@nogc nothrow @system extern (C) uint fsyscall(uint id, uint arg1, uint arg2, uint arg3);

nothrow @system extern (C) void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    uvprintf(format, args);
    va_end(args);
}

@nogc nothrow syscall_read(uint fd, uint buff, uint size) {
    return fsyscall(0x7, fd, buff, size);
}
