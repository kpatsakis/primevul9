dwarf_get_debug_str_index(Dwarf_Attribute attr,
    Dwarf_Unsigned *return_index,
    Dwarf_Error *error)
{
    int theform = attr->ar_attribute_form;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Debug dbg = 0;
    int res  = 0;
    Dwarf_Byte_Ptr section_end =  0;
    Dwarf_Unsigned index = 0;
    Dwarf_Small *info_ptr = 0;
    int indxres = 0;

    res = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    section_end =
        _dwarf_calculate_info_section_end_ptr(cu_context);
    info_ptr = attr->ar_debug_ptr;

    indxres = dw_read_str_index_val_itself(dbg, theform, info_ptr,
        section_end, &index,error);
    if (indxres == DW_DLV_OK) {
        *return_index = index;
        return indxres;
    }
    return indxres;
}