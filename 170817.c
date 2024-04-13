dwarf_convert_to_global_offset(Dwarf_Attribute attr,
    Dwarf_Off offset,
    Dwarf_Off * ret_offset,
    Dwarf_Error * error)
{
    Dwarf_Debug dbg = 0;
    Dwarf_CU_Context cu_context = 0;
    int res = 0;

    res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    switch (attr->ar_attribute_form) {
    case DW_FORM_ref1:
    case DW_FORM_ref2:
    case DW_FORM_ref4:
    case DW_FORM_ref8:
    case DW_FORM_ref_udata:
        /*  It is a cu-local offset. Convert to section-global. */
        /*  It would be nice to put some code to check
            legality of the offset */
        /*  cc_debug_offset always has any DWP Package File
            offset included (when the cu_context created)
            so there is no extra work for DWP.
            Globalize the offset */
        offset += cu_context->cc_debug_offset;

        break;

    case DW_FORM_ref_addr:
        /*  This offset is defined to be debug_info global already, so
            use this value unaltered.

            Since a DWP package file is not relocated there
            is no way that this reference offset to an address in
            any other CU can be correct for a DWP Package File offset
            */
        break;
    default: {
        dwarfstring m;

        dwarfstring_constructor(&m);
        dwarfstring_append_printf_u(&m,
            "DW_DLE_BAD_REF_FORM. The form "
            "code is 0x%x which cannot be converted to a global "
            " offset by "
            "dwarf_convert_to_global_offset()",
            attr->ar_attribute_form);
        _dwarf_error_string(dbg, error, DW_DLE_BAD_REF_FORM,
            dwarfstring_string(&m));
        dwarfstring_destructor(&m);
        return DW_DLV_ERROR;
        }
    }

    *ret_offset = (offset);
    return DW_DLV_OK;
}