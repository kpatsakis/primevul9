_dwarf_get_addr_index_itself(int theform,
    Dwarf_Small *info_ptr,
    Dwarf_Debug dbg,
    Dwarf_CU_Context cu_context,
    Dwarf_Unsigned *val_out,
    Dwarf_Error * error)
{
    Dwarf_Unsigned index = 0;
    Dwarf_Byte_Ptr section_end = 0;

    section_end =
        _dwarf_calculate_info_section_end_ptr(cu_context);
    switch(theform){
    case DW_FORM_LLVM_addrx_offset: {
        Dwarf_Unsigned tmp = 0;
        Dwarf_Unsigned tmp2 = 0;
        DECODE_LEB128_UWORD_CK(info_ptr,tmp,
            dbg,error,section_end);
        READ_UNALIGNED_CK(dbg, tmp2, Dwarf_Unsigned,
            info_ptr, SIZEOFT32,
            error,section_end);
        index = (tmp<<32) | tmp2;
        break;
    }
    case DW_FORM_GNU_addr_index:
    case DW_FORM_addrx:
        DECODE_LEB128_UWORD_CK(info_ptr,index,
            dbg,error,section_end);
        break;
    case DW_FORM_addrx1:
        READ_UNALIGNED_CK(dbg, index, Dwarf_Unsigned,
            info_ptr, 1,
            error,section_end);
        break;
    case DW_FORM_addrx2:
        READ_UNALIGNED_CK(dbg, index, Dwarf_Unsigned,
            info_ptr, 2,
            error,section_end);
        break;
    case DW_FORM_addrx3:
        READ_UNALIGNED_CK(dbg, index, Dwarf_Unsigned,
            info_ptr, 3,
            error,section_end);
        break;
    case DW_FORM_addrx4:
        READ_UNALIGNED_CK(dbg, index, Dwarf_Unsigned,
            info_ptr, 4,
            error,section_end);
        break;
    default:
        _dwarf_error(dbg, error, DW_DLE_ATTR_FORM_NOT_ADDR_INDEX);
        return DW_DLV_ERROR;
    }
    *val_out = index;
    return DW_DLV_OK;
}