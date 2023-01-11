module shell_d.main;

import core.vararg;

import libd;

import shell_d.arg;
import shell_d.vars;
import shell_d.utils;

struct LS_Args {
    Opt!bool help = {
        names: ["--help", ""],
        description: "this help           ",
    };

    mixin HelpedBase;

    Opt!bool list = {
        names: ["--list", "l"],
        description: "show files in rich table instead of oneline",
    };

    Opt!bool all = {
        names: ["--all", "a"],
        description: "show all files, including starting with . (hidden)",
    };

    Opt!bool human = {
        names: ["--human", "h"],
        description: "show sizes for human",
    };

    Arg!string path = {
        names: ["path", ""],
        description: "path to directory to get listed",
    };

    mixin PrintHelp!("ls", "prog to print ls");

    void validate() {
        if (this.path.value == "") {
            this.path.value = ".";
        }
    }
}

struct Set_Args {
    mixin HelpedBase;

    Opt!bool list = {
        names: ["--list", "l"],
        description: "list all variables with it's values",
    };

    Arg!string var_name = {
        names: ["var_name", ""],
        description: "name of var to be setted",
    };

    Arg!string var_value = {
        names: ["var_value", ""],
        description: "value of var to be setted",
    };

    mixin PrintHelp!("set", "prog to manipulate shell vars");

    void validate() {
    }
}

void cmd_ls(Split spl) {
    auto ls_args = LS_Args(spl.split()); // спасаемся от попадания нулевого аргумента (названия программы) в процесс парсинга
    if (ls_args.check_help())
        return;
}

void cmd_set(Split spl) {
    auto set_args = Set_Args(spl.split());

    if (set_args.check_help())
        return;

    if (set_args.list.value) {
        {
            auto tmp_str = to!string(shell_state.vars.debug_info);
            printf("debug                    = %S\n", tmp_str.ptr, tmp_str.length);
        }
        return;
    }

    if (set_args.var_name.value == "" || set_args.var_value.value == "") {
        printf("Empty var name/value\n");
        set_args.print_help();
        return;
    }

    switch (set_args.var_name.value) {
    case "debug":
    case "debug_info":
        bool new_val = to!bool(set_args.var_value.value);
        shell_state.vars.debug_info = new_val;
        break;
    default:
        printf("Unknown var name\n");
        break;
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
            cmd_ls(spl);
            break;
        case "set":
            cmd_set(spl);
            break;
        case "exit":
            syscall_ioctl(stdin, TTY_IOCTL.READ_MODE_ECHO_OFF, TTY_IOCTL.READ);
            break outer;
        case "clear":
            syscall_ioctl(stdin, TTY_IOCTL.CLEAR, TTY_IOCTL.WRITE);
            break;
        case "":
            break;
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
