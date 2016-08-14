global enable_paging
global set_page_dir
global flush_tlb_single

extern p_dir
enable_paging:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret

set_page_dir:
    mov eax,[p_dir]
    mov cr3,eax
    ret

flush_tlb_single:
    mov eax,[esp + 4]
    invlpg [0x1ff0000]