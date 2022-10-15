module shell_d.main;

import core.vararg;

import shell_d.libr;
import shell_d.utils;

void ls(Split spl) {
    int i = 0;
    auto spl_arg = spl;
    while (!spl_arg.isEmpty()) {
        auto arg = spl_arg.slice();
        printf("arg%d: %S\n", i, arg.ptr, arg.length);
        i += 1;
        spl_arg = spl_arg.split();
    }
}

extern (C) void main() {
    stdio_init();
    printf("Running repl in D\n");

    char[256] buff;

    outer: while (1) {
        printf("$ ");

        uint readen = syscall_read(stdin, cast(uint) buff.ptr, buff.length - 1);
        string s_buff = cast(immutable) buff[0 .. readen - 1];

        auto spl = split(s_buff, ' ');
        switch (spl.slice()) {
        case "ls":
            ls(spl);
            break;
        case "exit":
            break outer;
        default:
            printf("unknown cmd: '%S'\n", s_buff.ptr, s_buff.length);
            break;
        }
        // auto i = 0;
        // while (!spl.isEmpty()) {
        //     if (i == 0 && spl.slice() == "exit") {
        //         break outer;
        //     }
        //     printf("%d-th arg: %s\n", i, spl.slice().ptr);
        //     spl = spl.split();
        //     i += 1;
        // }
    }
}
