get_attr_dbg(Dwarf_Debug *dbg_out,
    Dwarf_CU_Context * cu_context_out,
    Dwarf_Attribute attr,
    Dwarf_Error *error)
{
    Dwarf_CU_Context cup = 0;
    Dwarf_Debug dbg = 0;

    if (!attr) {
        _dwarf_error(NULL, error, DW_DLE_ATTR_NULL);
        return DW_DLV_ERROR;
    }
    cup = attr->ar_cu_context;
    if (!cup) {
        _dwarf_error(NULL, error, DW_DLE_ATTR_NO_CU_CONTEXT);
        return DW_DLV_ERROR;
    }
    dbg = cup->cc_dbg;
    if (!dbg  || dbg->de_magic != DBG_IS_VALID) {
        _dwarf_error_string(NULL, error, DW_DLE_ATTR_DBG_NULL,
            "DW_DLE_ATTR_DBG_NULL: Stale or null Dwarf_Debug"
            "in a Dwarf_CU_Context" );
        return DW_DLV_ERROR;
    }
    if (dbg != attr->ar_dbg) {
        _dwarf_error_string(NULL, error, DW_DLE_ATTR_DBG_NULL,
            "DW_DLE_ATTR_DBG_NULL: an attribute and its "
            "cu_context do not have the same Dwarf_Debug" );
        return DW_DLV_ERROR;
    }
    *cu_context_out = cup;
    *dbg_out        = dbg;
    return DW_DLV_OK;

}