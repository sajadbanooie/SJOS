global enable_paging
global set_page_dir

extern p_dir
enable_paging:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret

set_page_dir:
    mov eax,p_dir
    mov cr3,eax
    ret