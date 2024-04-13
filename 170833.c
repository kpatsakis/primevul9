_dwarf_extract_local_debug_str_string_given_offset(Dwarf_Debug dbg,
    unsigned attrform,
    Dwarf_Unsigned offset,
    char ** return_str,
    Dwarf_Error * error)
{
    if (attrform == DW_FORM_strp ||
        attrform == DW_FORM_line_strp ||
        attrform == DW_FORM_GNU_str_index ||
        attrform == DW_FORM_strx1 ||
        attrform == DW_FORM_strx2 ||
        attrform == DW_FORM_strx3 ||
        attrform == DW_FORM_strx4 ||
        attrform == DW_FORM_strx) {
        /*  The 'offset' into .debug_str or .debug_line_str is given,
            here we turn that into a pointer. */
        Dwarf_Small   *secend = 0;
        Dwarf_Small   *secbegin = 0;
        Dwarf_Small   *strbegin = 0;
        Dwarf_Unsigned secsize = 0;
        int errcode = 0;
        const char *errname = 0;
        int res = 0;

        if (attrform == DW_FORM_line_strp) {
            res = _dwarf_load_section(dbg,
                &dbg->de_debug_line_str,error);
            if (res != DW_DLV_OK) {
                return res;
            }
            errcode = DW_DLE_STRP_OFFSET_BAD;
            errname = "DW_DLE_STRP_OFFSET_BAD";
            secsize = dbg->de_debug_line_str.dss_size;
            secbegin = dbg->de_debug_line_str.dss_data;
            strbegin= dbg->de_debug_line_str.dss_data + offset;
            secend = dbg->de_debug_line_str.dss_data + secsize;
        } else {
            /* DW_FORM_strp  etc */
            res = _dwarf_load_section(dbg, &dbg->de_debug_str,error);
            if (res != DW_DLV_OK) {
                return res;
            }
            errcode = DW_DLE_STRING_OFFSET_BAD;
            errname = "DW_DLE_STRING_OFFSET_BAD";
            secsize = dbg->de_debug_str.dss_size;
            secbegin = dbg->de_debug_str.dss_data;
            strbegin= dbg->de_debug_str.dss_data + offset;
            secend = dbg->de_debug_str.dss_data + secsize;
        }
        if (offset >= secsize) {
            dwarfstring m;
            const char *name = "<unknownform>";

            dwarf_get_FORM_name(attrform,&name);

            dwarfstring_constructor(&m);
            dwarfstring_append(&m,(char *)errname);
            dwarfstring_append_printf_s(&m,
                " Form %s ",(char *)name);
            dwarfstring_append_printf_u(&m,
                "string offset of 0x%" DW_PR_DUx " ",
                offset);
            dwarfstring_append_printf_u(&m,
                "is larger than the string section "
                "size of  0x%" DW_PR_DUx,
                secsize);
            _dwarf_error_string(dbg, error, errcode,
                dwarfstring_string(&m));
            dwarfstring_destructor(&m);
            /*  Badly damaged DWARF here. */
            return DW_DLV_ERROR;
        }
        res= _dwarf_check_string_valid(dbg,secbegin,strbegin, secend,
            errcode,error);
        if (res != DW_DLV_OK) {
            return res;
        }

        *return_str = (char *)strbegin;
        return DW_DLV_OK;
    }
    generate_form_error(dbg,error,attrform,
        DW_DLE_ATTR_FORM_BAD,
        "DW_DLE_ATTR_FORM_BAD",
        "extract debug_str string");
    return DW_DLV_ERROR;
}