        .k1: ; TODO: разобраться, как эта херня работает
             mov [edi], eax
             add eax, 0x1000
             add edi, 4
             dec ecx
             jnz .k1

             
            mov edx, 0x003
            lea ecx, [(0x100000000 - VIRT_BASE) >> 12]
        .k2:
            mov [edi], edx
            add eax, 0x1000
            add edi, 4
            dec ecx
            jnz .k2

            lea edi, [PHYS(boot_page_directory)]
            lea edx, [PHYS(boot_page_table) + 0x003]
            mov ecx, 0x400
        .k3: 
            mov [edi], edx
            add edx, 0x1000
            add edi, 4
            dec ecx
            jnz .k3
