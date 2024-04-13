dwarf_formexprloc(Dwarf_Attribute attr,
    Dwarf_Unsigned * return_exprlen,
    Dwarf_Ptr  * block_ptr,
    Dwarf_Error * error)
{
    Dwarf_Debug dbg = 0;
    Dwarf_CU_Context cu_context = 0;

    int res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    if (attr->ar_attribute_form == DW_FORM_exprloc ) {
        Dwarf_Die die = 0;
        Dwarf_Unsigned leb_len = 0;
        Dwarf_Byte_Ptr section_start = 0;
        Dwarf_Unsigned section_len = 0;
        Dwarf_Byte_Ptr section_end = 0;
        Dwarf_Byte_Ptr info_ptr = 0;
        Dwarf_Unsigned exprlen = 0;
        Dwarf_Small * addr = attr->ar_debug_ptr;

        info_ptr = addr;
        section_start =
            _dwarf_calculate_info_section_start_ptr(cu_context,
            &section_len);
        section_end = section_start + section_len;

        DECODE_LEB128_UWORD_LEN_CK(info_ptr, exprlen, leb_len,
            dbg,error,section_end);
        if (exprlen > section_len) {
            /* Corrupted dwarf!  */
            dwarfstring m;

            dwarfstring_constructor(&m);
            dwarfstring_append_printf_u(&m,
                "DW_DLE_ATTR_OUTSIDE_SECTION: "
                "The expression length is %u,",exprlen);
            dwarfstring_append_printf_u(&m,
                " but the section length is just %u. "
                "Corrupt Dwarf.",section_len);
            _dwarf_error_string(dbg, error,
                DW_DLE_ATTR_OUTSIDE_SECTION,
                dwarfstring_string(&m));
            dwarfstring_destructor(&m);
            return DW_DLV_ERROR;
        }
        die = attr->ar_die;
        /*  Is the block entirely in the section, or is
            there bug somewhere?
            Here the final addr may be 1 past end of section. */
        if (_dwarf_reference_outside_section(die,
            (Dwarf_Small *)addr,
            ((Dwarf_Small *)addr)+exprlen +leb_len)) {
            dwarfstring m;

            dwarfstring_constructor(&m);
            dwarfstring_append_printf_u(&m,
                "DW_DLE_ATTR_OUTSIDE_SECTION: "
                "The expression length %u,",exprlen);
            dwarfstring_append_printf_u(&m,
                " plus the leb value length of "
                "%u ",leb_len);
            dwarfstring_append(&m,
                " runs past the end of the section. "
                "Corrupt Dwarf.");
            _dwarf_error_string(dbg, error,
                DW_DLE_ATTR_OUTSIDE_SECTION,
                dwarfstring_string(&m));
            dwarfstring_destructor(&m);
            return DW_DLV_ERROR;
        }
        *return_exprlen = exprlen;
        *block_ptr = addr + leb_len;
        return DW_DLV_OK;

    }
    {
        dwarfstring m;
        const char *name = "<name not known>";
        unsigned  mform = attr->ar_attribute_form;

        dwarfstring_constructor(&m);

        dwarf_get_FORM_name (mform,&name);
        dwarfstring_append_printf_u(&m,
            "DW_DLE_ATTR_EXPRLOC_FORM_BAD: "
            "The form is 0x%x ", mform);
        dwarfstring_append_printf_s(&m,
            "(%s) but should be DW_FORM_exprloc. "
            "Corrupt Dwarf.",(char *)name);
        _dwarf_error_string(dbg, error, DW_DLE_ATTR_EXPRLOC_FORM_BAD,
            dwarfstring_string(&m));
        dwarfstring_destructor(&m);
    }
    return DW_DLV_ERROR;
}