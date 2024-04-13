dwarf_addr_form_is_indexed(int form)
{
    switch(form) {
    case DW_FORM_addrx:
    case DW_FORM_addrx1:
    case DW_FORM_addrx2:
    case DW_FORM_addrx3:
    case DW_FORM_addrx4:
    case DW_FORM_GNU_addr_index:
    case DW_FORM_LLVM_addrx_offset:
        return TRUE;
    default: break;
    }
    return FALSE;
}