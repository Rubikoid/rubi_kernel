module shell_d.utils;

version (unittest) {
    import core.stdc.stdio : printf, sprintf;

    // extern (C) void main() {
    //     static foreach (u; __traits(getUnitTests, __traits(parent, main)))
    //         u();
    // }    
} else {
    import shell_d.libr : printf, sprintf;
}

nothrow void print_slice(string slice) {
    char[10] printf_prepare;
    sprintf(printf_prepare.ptr, "%%%ds\n", slice.length);
    printf(printf_prepare.ptr, slice.ptr);
}

struct Split {
    private string raw_slice;
    char sep;
    uint start;
    uint end;

    @safe @nogc nothrow pure Split split() {
        if (end + 1 >= raw_slice.length) {
            return Split(raw_slice, sep, end, end);
        }
        immutable auto local_start = end + 1;
        immutable auto local_slice = raw_slice[local_start .. $];
        uint iter = 0;

        if (local_slice.length == 0) {
            return Split(raw_slice, sep, local_start, local_start);
        }

        while (iter < local_slice.length && local_slice[iter] != sep) {
            iter++;
        }

        return Split(raw_slice, sep, local_start, local_start + iter);
    }

    @safe @nogc nothrow pure string slice() {
        return raw_slice[start .. end];
    }

    @safe @nogc nothrow pure bool isEmpty() {
        return start == end;
    }

    nothrow void print() {
        printf("Slice of '%S' (%d), sep='%c', start=%d, end=%d, empty=%d, slice='%S' (%d)\n",
            raw_slice.ptr,
            raw_slice.length,
            raw_slice.length,
            sep,
            start,
            end,
            isEmpty(),
            slice().ptr,
            slice().length,
            slice().length);
        // print_slice();
    }
}

@safe @nogc nothrow pure Split split(string buff, immutable char sep, immutable uint start = 0) {
    return Split(buff, sep, start, -1).split();
}

@safe @nogc nothrow pure unittest {
    auto src1 = "ab cd ef gh";
    auto s1 = split(src1, ' ');

    assert(s1.slice() == "ab");
    assert(s1.slice().ptr == src1.ptr);
    assert(s1.slice() != "cd");

    auto s2 = split("ab", ' ');
    assert(s2.slice() != "a");
    assert(s2.slice() == "ab");

    auto s3 = split("aa ab", ' ');
    assert(s3.slice() != "ab");
    assert(s3.slice() == "aa");

    auto s4 = s1.split();
    // s1.print();
    // s4.print();
    assert(s4.slice() != "ab");
    assert(s4.slice() == "cd");
    assert(s4.slice().ptr != src1.ptr);
    assert(s4.slice().ptr == src1[3 .. $].ptr);

    auto s5 = s3.split();
    assert(s5.slice() == "ab");

    auto s6 = s5.split();
    assert(s6.slice() == "");
    assert(s6.isEmpty() == true);

    auto s7 = s2.split();
    assert(s7.slice() == "");
    assert(s7.isEmpty() == true);
}
