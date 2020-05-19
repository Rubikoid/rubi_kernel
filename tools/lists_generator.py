def do_magic(t, pref):
    def_format = "#define {pref}_{name} 0x{id:02X}\n"
    arr_head_format = "static const char *{pref}_names[{PREF}_COUNT] = {{\n{body}\n}};"
    arr_format = '"{data}",\n'

    defs = ""
    arr = ""

    if isinstance(t, list):
        t = {v: i for i, v in enumerate(t)}
    else:
        t = {i: int(t[i], base=0) for i in t}
    rev_t = {t[i]: i for i in t}

    for i in sorted(t.keys(), key=lambda x: t[x]):
        defs += def_format.format(pref=pref.upper(), name=i.upper(), id=t[i])

    for i in range(max(t.values()) + 1):
        if i in rev_t:
            arr += arr_format.format(data=rev_t[i].upper())
        else:
            arr += arr_format.format(data=f"{pref.upper()}_NANI")

    arr = arr_head_format.format(body=arr, pref=pref.lower(), PREF=pref.upper())
    defs += def_format.format(pref=pref.upper(), name="COUNT", id=max(t.values()) + 1)
    print(defs)
    print(arr)



