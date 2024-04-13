dwarf_get_debug_addr_index(Dwarf_Attribute attr,
    Dwarf_Unsigned * return_index,
    Dwarf_Error * error)
{
    int theform = 0;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Debug dbg = 0;

    int res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    theform = attr->ar_attribute_form;
    if (dwarf_addr_form_is_indexed(theform)) {
        Dwarf_Unsigned index = 0;

        res = _dwarf_get_addr_index_itself(theform,
            attr->ar_debug_ptr,dbg,cu_context,&index,error);
        *return_index = index;
        return res;
    }

    _dwarf_error(dbg, error, DW_DLE_ATTR_FORM_NOT_ADDR_INDEX);
    return DW_DLV_ERROR;
}