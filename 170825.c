_dwarf_allow_formudata(unsigned form)
{
    switch(form) {
    case DW_FORM_data1:
    case DW_FORM_data2:
    case DW_FORM_data4:
    case DW_FORM_data8:
    case DW_FORM_udata:
    case DW_FORM_loclistx:
    case DW_FORM_rnglistx:
        return TRUE;
    default:
        break;
    }
    return FALSE;
}