module shell_d.vars;

struct ShellVars {
    bool debug_info = false;
}

struct ShellState {
    ShellVars vars;
}

shared ShellState shell_state;
