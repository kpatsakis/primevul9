_dwarf_formsig8_internal(Dwarf_Attribute attr,
    int formexpected,
    Dwarf_Sig8 * returned_sig_bytes,
    Dwarf_Error*     error)
{
    Dwarf_Debug dbg = 0;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Byte_Ptr  field_end = 0;
    Dwarf_Byte_Ptr  section_end = 0;

    int res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }

    if (attr->ar_attribute_form != formexpected) {
        return DW_DLV_NO_ENTRY;
    }
    section_end =
        _dwarf_calculate_info_section_end_ptr(cu_context);
    field_end = attr->ar_debug_ptr + sizeof(Dwarf_Sig8);
    if (field_end > section_end) {
        _dwarf_error(dbg, error, DW_DLE_ATTR_FORM_OFFSET_BAD);
        return DW_DLV_ERROR;
    }

    memcpy(returned_sig_bytes, attr->ar_debug_ptr,
        sizeof(*returned_sig_bytes));
    return DW_DLV_OK;
}