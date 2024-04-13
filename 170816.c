dwarf_formdata16(Dwarf_Attribute attr,
    Dwarf_Form_Data16  * returned_val,
    Dwarf_Error*     error)
{
    Dwarf_Half attrform = 0;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Debug dbg = 0;
    int res  = 0;
    Dwarf_Small *section_end = 0;
    Dwarf_Unsigned section_length = 0;
    Dwarf_Small *section_start = 0;

    if (!attr) {
        _dwarf_error(NULL, error, DW_DLE_ATTR_NULL);
        return DW_DLV_ERROR;
    }
    if (!returned_val ) {
        _dwarf_error(NULL, error, DW_DLE_ATTR_NULL);
        return DW_DLV_ERROR;
    }
    res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    attrform = attr->ar_attribute_form;
    if (attrform != DW_FORM_data16) {
        generate_form_error(dbg,error,attrform,
            DW_DLE_ATTR_FORM_BAD,
            "DW_DLE_ATTR_FORM_BAD",
            "dwarf_formdata16");
        return DW_DLV_ERROR;
    }
    section_start = _dwarf_calculate_info_section_start_ptr(
        cu_context,&section_length);
    section_end = section_start + section_length;

    res = _dwarf_extract_data16(dbg, attr->ar_debug_ptr,
        section_start, section_end,
        returned_val,  error);
    return res;
}