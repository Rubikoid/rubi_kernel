def from_simple_list(l, pref):
    f = {}
    for i in l.split("\n"):
        i = i.strip()
        if not i:
            continue
        ii = i.split("\t")
        ii[1] = ii[1].replace(" ", "_")
        f[ii[1]] = str(int(ii[0], base=16))
    do_magic(f, pref)


def do_pci_file():
    d = open("pci.txt", 'r').read()
    d = d.split("!!!")
    for i in d:
        i = i.strip()
        ii = i.split(":")
        from_simple_list(ii[1], ii[0])


def do_magic(t, pref):
    def_format = "#define {pref}_{name} 0x{id:02X}\n"
    arr_head_format = "static const char *{pref}_names[{PREF}_COUNT] = {{\n{body} }};"
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


do_pci_file()
