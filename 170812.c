_dwarf_get_string_from_tied(Dwarf_Debug dbg,
    Dwarf_Unsigned offset,
    char **return_str,
    Dwarf_Error*error)
{
    Dwarf_Debug tieddbg = 0;
    Dwarf_Small *secend = 0;
    Dwarf_Small *secbegin = 0;
    Dwarf_Small *strbegin = 0;
    int res = DW_DLV_ERROR;
    Dwarf_Error localerror = 0;

    /* Attach errors to dbg, not tieddbg. */
    tieddbg = dbg->de_tied_data.td_tied_object;
    if (!tieddbg) {
        _dwarf_error(dbg, error, DW_DLE_NO_TIED_FILE_AVAILABLE);
        return  DW_DLV_ERROR;
    }
    /* The 'offset' into .debug_str is set. */
    res = _dwarf_load_section(tieddbg, &tieddbg->de_debug_str,
        &localerror);
    if (res == DW_DLV_ERROR) {
        Dwarf_Unsigned lerrno = dwarf_errno(localerror);
        dwarf_dealloc(tieddbg,localerror,DW_DLA_ERROR);
        _dwarf_error(dbg,error,lerrno);
        return res;
    }
    if (res == DW_DLV_NO_ENTRY) {
        return res;
    }
    if (offset >= tieddbg->de_debug_str.dss_size) {
        /*  Badly damaged DWARF here. */
        _dwarf_error(dbg, error,  DW_DLE_NO_TIED_STRING_AVAILABLE);
        return DW_DLV_ERROR;
    }
    secbegin = tieddbg->de_debug_str.dss_data;
    strbegin= tieddbg->de_debug_str.dss_data + offset;
    secend = tieddbg->de_debug_str.dss_data +
        tieddbg->de_debug_str.dss_size;

    /*  Ensure the offset lies within the .debug_str */
    if (offset >= tieddbg->de_debug_str.dss_size) {
        _dwarf_error(dbg, error,  DW_DLE_NO_TIED_STRING_AVAILABLE);
        return DW_DLV_ERROR;
    }
    res= _dwarf_check_string_valid(tieddbg,secbegin,strbegin, secend,
        DW_DLE_NO_TIED_STRING_AVAILABLE,
        &localerror);
    if (res == DW_DLV_ERROR) {
        Dwarf_Unsigned lerrno = dwarf_errno(localerror);
        dwarf_dealloc(tieddbg,localerror,DW_DLA_ERROR);
        _dwarf_error(dbg,error,lerrno);
        return res;
    }
    if (res == DW_DLV_NO_ENTRY) {
        return res;
    }
    *return_str = (char *) (tieddbg->de_debug_str.dss_data + offset);
    return DW_DLV_OK;
}