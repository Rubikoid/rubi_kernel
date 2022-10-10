module shell_d.main;

import core.vararg;

import shell_d.libr;
import shell_d.utils;

void ls(Split spl) {

}

extern (C) void main() {
    stdio_init();
    printf("Running repl in D\n");

    char[256] buff;

    outer: while (1) {
        printf("$ ");
        uint readen = syscall_read(stdin, cast(uint) buff.ptr, buff.length - 1);
        buff[readen - 1] = 0;

        auto spl = split(cast(immutable) buff, ' ');
        switch (spl.slice()) {
        case "ls":
            ls(spl);
            break;
        case "exit":
            break outer;
        default:
            printf("unknown cmd: '%s'", buff.ptr);
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
