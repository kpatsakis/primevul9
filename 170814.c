dwarf_formaddr(Dwarf_Attribute attr,
    Dwarf_Addr * return_addr, Dwarf_Error * error)
{
    Dwarf_Debug dbg = 0;
    Dwarf_Addr ret_addr = 0;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Half attrform = 0;
    int res = 0;

    res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    attrform = attr->ar_attribute_form;
    if (dwarf_addr_form_is_indexed(attrform)) {
        res = _dwarf_look_in_local_and_tied(
            attrform,
            cu_context,
            attr->ar_debug_ptr,
            return_addr,
            error);
        return res;
    }
    if (attrform == DW_FORM_addr
        /*  || attrform == DW_FORM_ref_addr Allowance of
            DW_FORM_ref_addr was a mistake. The value returned in that
            case is NOT an address it is a global debug_info
            offset (ie, not CU-relative offset within the CU
            in debug_info).
            The DWARF2 document refers to it as an address
            (misleadingly) in sec 6.5.4 where it describes
            the reference form. It is
            address-sized so that the linker can easily update it, but
            it is a reference inside the debug_info section. No longer
            allowed. */
        ) {
        Dwarf_Small *section_end =
            _dwarf_calculate_info_section_end_ptr(cu_context);

        READ_UNALIGNED_CK(dbg, ret_addr, Dwarf_Addr,
            attr->ar_debug_ptr,
            cu_context->cc_address_size,
            error,section_end);
        *return_addr = ret_addr;
        return DW_DLV_OK;
    }
    generate_form_error(dbg,error,attrform,
        DW_DLE_ATTR_FORM_BAD,
        "DW_DLE_ATTR_FORM_BAD",
        "dwarf_formaddr");
    return DW_DLV_ERROR;
}