dw_read_str_index_val_itself(Dwarf_Debug dbg,
    unsigned theform,
    Dwarf_Small *info_ptr,
    Dwarf_Small *section_end,
    Dwarf_Unsigned *return_index,
    Dwarf_Error *error)
{
    Dwarf_Unsigned index = 0;

    switch(theform) {
    case DW_FORM_strx:
    case DW_FORM_GNU_str_index:
        DECODE_LEB128_UWORD_CK(info_ptr,index,
            dbg,error,section_end);
        break;
    case DW_FORM_strx1:
        READ_UNALIGNED_CK(dbg, index, Dwarf_Unsigned,
            info_ptr, 1,
            error,section_end);
        break;
    case DW_FORM_strx2:
        READ_UNALIGNED_CK(dbg, index, Dwarf_Unsigned,
            info_ptr, 2,
            error,section_end);
        break;
    case DW_FORM_strx3:
        READ_UNALIGNED_CK(dbg, index, Dwarf_Unsigned,
            info_ptr, 3,
            error,section_end);
        break;
    case DW_FORM_strx4:
        READ_UNALIGNED_CK(dbg, index, Dwarf_Unsigned,
            info_ptr, 4,
            error,section_end);
        break;
    default:
        _dwarf_error(dbg, error, DW_DLE_ATTR_FORM_NOT_STR_INDEX);
        return DW_DLV_ERROR;
    }
    *return_index = index;
    return DW_DLV_OK;
}