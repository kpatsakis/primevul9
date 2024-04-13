dwarf_formudata(Dwarf_Attribute attr,
    Dwarf_Unsigned * return_uval, Dwarf_Error * error)
{
    Dwarf_Debug dbg = 0;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Byte_Ptr section_end = 0;
    Dwarf_Unsigned bytes_read = 0;
    Dwarf_Byte_Ptr data =  attr->ar_debug_ptr;
    unsigned form = 0;

    int res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    section_end =
        _dwarf_calculate_info_section_end_ptr(cu_context);
    form = attr->ar_attribute_form;

    res = _dwarf_formudata_internal(dbg,
        attr,
        form, data, section_end, return_uval,
        &bytes_read, error);
    return res;
}