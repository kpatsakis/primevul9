dwarf_formflag(Dwarf_Attribute attr,
    Dwarf_Bool * ret_bool, Dwarf_Error * error)
{
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Debug dbg = 0;

    if (!attr) {
        _dwarf_error(NULL, error, DW_DLE_ATTR_NULL);
        return DW_DLV_ERROR;
    }
    cu_context = attr->ar_cu_context;
    if (!cu_context) {
        _dwarf_error(NULL, error, DW_DLE_ATTR_NO_CU_CONTEXT);
        return DW_DLV_ERROR;
    }
    dbg = cu_context->cc_dbg;
    if (!dbg || dbg->de_magic != DBG_IS_VALID) {
        _dwarf_error_string(NULL, error, DW_DLE_ATTR_DBG_NULL,
            "DW_DLE_ATTR_DBG_NULL: dwarf_formflag() attribute"
            " passed in has NULL or stale Dwarf_Debug pointer");
        return DW_DLV_ERROR;
    }
    if (dbg != attr->ar_dbg) {
        _dwarf_error_string(NULL, error, DW_DLE_ATTR_DBG_NULL,
            "DW_DLE_ATTR_DBG_NULL: an attribute and its "
            "cu_context do not have the same Dwarf_Debug" );
        return DW_DLV_ERROR;
    }
    if (attr->ar_attribute_form == DW_FORM_flag_present) {
        /*  Implicit means we don't read any data at all. Just
            the existence of the Form does it. DWARF4. */
        *ret_bool = 1;
        return DW_DLV_OK;
    }

    if (attr->ar_attribute_form == DW_FORM_flag) {
        *ret_bool = *(Dwarf_Small *)(attr->ar_debug_ptr);
        return DW_DLV_OK;
    }
    generate_form_error(dbg,error,attr->ar_attribute_form,
        DW_DLE_ATTR_FORM_BAD,
        "DW_DLE_ATTR_FORM_BAD",
        "dwarf_formflat");
    return DW_DLV_ERROR;
}