dwarf_formref(Dwarf_Attribute attr,
    Dwarf_Off * ret_offset,
    Dwarf_Bool * ret_is_info,
    Dwarf_Error * error)
{
    Dwarf_Debug dbg = 0;
    Dwarf_Unsigned offset = 0;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Unsigned maximumoffset = 0;
    int res = DW_DLV_ERROR;
    Dwarf_Byte_Ptr section_end = 0;
    Dwarf_Bool is_info = TRUE;

    *ret_offset = 0;
    res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    section_end =
        _dwarf_calculate_info_section_end_ptr(cu_context);
    is_info = cu_context->cc_is_info;

    switch (attr->ar_attribute_form) {

    case DW_FORM_ref1:
        offset = *(Dwarf_Small *) attr->ar_debug_ptr;
        break;

    case DW_FORM_ref2:
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_HALF_SIZE,
            error,section_end);
        break;

    case DW_FORM_ref4:
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_32BIT_SIZE,
            error,section_end);
        break;

    case DW_FORM_ref8:
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_64BIT_SIZE,
            error,section_end);
        break;

    case DW_FORM_ref_udata: {
        Dwarf_Byte_Ptr ptr = attr->ar_debug_ptr;
        Dwarf_Unsigned localoffset = 0;

        DECODE_LEB128_UWORD_CK(ptr,localoffset,
            dbg,error,section_end);
        offset = localoffset;
        break;
    }
    case DW_FORM_ref_sig8: {
        /*  We need to look for a local reference here.
            The function we are in is only CU_local
            offsets returned. */
#if 0
        Dwarf_Sig8 sig8;
        memcpy(&sig8,ptr,sizeof(Dwarf_Sig8));
        res = dwarf_find_die_given_sig8(dbg,
            &sig8, ...
        We could look, then determine if
        resulting offset is actually local.
#endif /*0*/

        /*  We cannot handle this here.
            The reference could be to .debug_types
            or another CU!
            not a .debug_info CU local offset. */
        _dwarf_error(dbg, error, DW_DLE_REF_SIG8_NOT_HANDLED);
        return DW_DLV_ERROR;
    }
    default: {
        dwarfstring m;

        dwarfstring_constructor(&m);
        dwarfstring_append_printf_u(&m,
            "DW_DLE_BAD_REF_FORM. The form "
            "code is 0x%x which does not have an offset "
            " for "
            "dwarf_formref() to return.",
            attr->ar_attribute_form);
        _dwarf_error_string(dbg, error, DW_DLE_BAD_REF_FORM,
            dwarfstring_string(&m));
        dwarfstring_destructor(&m);
        return DW_DLV_ERROR;
        }
    }

    /*  Check that offset is within current
        cu portion of .debug_info. */

    maximumoffset = cu_context->cc_length +
        cu_context->cc_length_size +
        cu_context->cc_extension_size;
    if (offset >= maximumoffset) {
        /*  For the DW_TAG_compile_unit is legal to have the
            DW_AT_sibling attribute outside the current cu portion of
            .debug_info.
            In other words, sibling points to the end of the CU.
            It is used for precompiled headers.
            The valid condition will be: 'offset == maximumoffset'. */
        Dwarf_Half tag = 0;
        int tres = dwarf_tag(attr->ar_die,&tag,error);
        if (tres != DW_DLV_OK) {
            if (tres == DW_DLV_NO_ENTRY) {
                _dwarf_error(dbg, error, DW_DLE_NO_TAG_FOR_DIE);
                return DW_DLV_ERROR;
            }
            return DW_DLV_ERROR;
        }

        if (DW_TAG_compile_unit != tag &&
            DW_AT_sibling != attr->ar_attribute &&
            offset > maximumoffset) {
            _dwarf_error(dbg, error, DW_DLE_ATTR_FORM_OFFSET_BAD);
            /*  Return the incorrect offset for better
                error reporting */
            *ret_offset = (offset);
            return DW_DLV_ERROR;
        }
    }
    *ret_is_info = is_info;
    *ret_offset = (offset);
    return DW_DLV_OK;
}