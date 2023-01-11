module shell_d.arg;

import libd;

import shell_d.vars;
import shell_d.utils;

mixin template ArgBase(T) {
    string[2] names;
    string description;
    T value;

    bool isArg() {
        return false;
    }

    bool check(ref Split spl, int pos) {
        return false;
    }

    void print() {
        printf("%S: ", this.names[0].ptr, this.names[0].length);
        static if (is(T : bool)) {
            if (this.value)
                printf("true");
            else
                printf("false");
        } else static if (is(T : string)) {
            if (this.value != "")
                printf("'%S'", this.value.ptr, this.value.length);
            else
                printf("''");
        } else static if (is(T : int)) {
            printf("%d", this.value);
        }
        printf("\n");
    }
}

mixin template HelpedBase() {
    Opt!bool help = {
        names: ["--help", "h"],
        description: "this help           ",
    };

    bool check_help() {
        if (this.help.value) {
            this.print_help();
            return true;
        }

        if (shell_state.vars.debug_info)
            this.print_args();

        return false;
    }

    mixin ParseArgs;
    mixin PrintArgs;
}

struct Opt(T) {
    mixin ArgBase!T;

    bool isArg() {
        return false;
    }

    bool check(ref Split spl, int pos) {
        bool do_check = false;

        // printf("Do check on '%S', pos=%d, slice=", names[0].ptr, names[0].length, pos);
        // spl.print();

        if (pos == 0) {
            do_check = names[pos] != "" && spl.slice() == names[pos];
        } else if (pos > 0) {
            do_check = names[1] != "" && spl.slice()[pos .. pos + 1] == names[1];
        }
        if (do_check) {
            static if (is(T : bool)) {
                this.value = true;
            } else static if (is(T : string)) {
                spl = spl.split();
                this.value = spl.slice();
            } else static if (is(T : int)) {
                spl = spl.split();
                this.value = to!int(spl.slice());
            }
            return true;
        }
        return false;
    }
}

struct Arg(T) {
    mixin ArgBase!T;

    bool isArg() {
        return true;
    }

    bool check(ref Split spl, int pos) {
        static if (is(T : string)) {
            if (this.value == "") {
                this.value = spl.slice();
                return true;
            }
        }
        return false;
    }
}

mixin template ParseArgs() {
    this(Split spl) {
        int i = 0;

        outer: for (auto spl_arg = spl; !spl_arg.isEmpty(); i += 1, spl_arg = spl_arg.split()) {
            auto arg = spl_arg.slice();

            // printf("Resolving arg%d: '%S'\n", i, arg.ptr, arg.length);

            // check for --option
            if (arg[0 .. 2] == "--") {
                foreach (ref arg_to_check; this.tupleof) {
                    if (!arg_to_check.isArg() && arg_to_check.check(spl_arg, 0)) {
                        // printf("Arg found: '%S'\n", arg.ptr, arg.length);
                        continue outer;
                    }
                }
            }

            // printf("-- not found, going to -\n", arg.ptr, arg.length);

            // check for -o / -oabc
            if (arg[0] == '-') {
                auto flag = false;
                for (auto j = 1; j < arg.length; j++) {
                    foreach (ref arg_to_check; this.tupleof) {
                        if (!arg_to_check.isArg() && arg_to_check.check(spl_arg, 1)) {
                            flag = true;
                            break;
                        }
                    }
                }
                if (flag) {
                    continue;
                }
            }

            // printf("- not found, going to arguments\n", arg.ptr, arg.length);

            // check for other args
            foreach (ref arg_to_check; this.tupleof) {
                if (arg_to_check.isArg() && arg_to_check.check(spl_arg, 0)) {
                    continue outer;
                }
            }

            printf("unknown arg%d: %S\n", i, arg.ptr, arg.length);
        }

        this.validate();
    }

    void validate() {
    }
}

mixin template PrintHelp(string prog_name, string prog_desc) {
    void print_help() {
        printf("Usage: " ~ prog_name ~ " [OPTION...]");
        foreach (ref arg_to_check; this.tupleof) {
            if (arg_to_check.isArg()) {
                printf(" [%S]", arg_to_check.names[0].ptr, arg_to_check.names[0].length);
            }
        }
        printf("\nDesc: ");
        printf(prog_desc ~ "\n");
        printf("Help: \n");
        foreach (ref arg_to_check; this.tupleof) {
            if (arg_to_check.isArg()) {
                printf(
                    "  [%S] - %S\n",
                    arg_to_check.names[0].ptr, arg_to_check.names[0].length,
                    arg_to_check.description.ptr, arg_to_check.description.length,
                );
            } else {
                if (arg_to_check.names[1] != "") {
                    printf(
                        "  -%S %S - %S\n",
                        arg_to_check.names[1].ptr, arg_to_check.names[1].length,
                        arg_to_check.names[0].ptr, arg_to_check.names[0].length,
                        arg_to_check.description.ptr, arg_to_check.description.length,
                    );
                } else {
                    printf(
                        "  %S - %S\n",
                        arg_to_check.names[0].ptr, arg_to_check.names[0].length,
                        arg_to_check.description.ptr, arg_to_check.description.length,
                    );
                }
            }
        }
    }
}

mixin template PrintArgs() {
    void print_args() {
        foreach (ref arg_to_check; this.tupleof) {
            arg_to_check.print();
        }
    }
}
