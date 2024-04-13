dwarf_global_formref_b(Dwarf_Attribute attr,
    Dwarf_Off * ret_offset,
    Dwarf_Bool * offset_is_info,
    Dwarf_Error * error)
{
    Dwarf_Debug dbg = 0;
    Dwarf_Unsigned offset = 0;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Half context_version = 0;
    Dwarf_Byte_Ptr section_end = 0;
    Dwarf_Bool is_info = TRUE;

    int res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    section_end =
        _dwarf_calculate_info_section_end_ptr(cu_context);
    context_version = cu_context->cc_version_stamp;
    is_info = cu_context->cc_is_info;
    switch (attr->ar_attribute_form) {

    case DW_FORM_ref1:
        offset = *(Dwarf_Small *) attr->ar_debug_ptr;
        goto fixoffset;

    case DW_FORM_ref2:
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_HALF_SIZE,
            error,section_end);
        goto fixoffset;

    case DW_FORM_ref4:
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_32BIT_SIZE,
            error,section_end);
        goto fixoffset;

    case DW_FORM_ref8:
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_64BIT_SIZE,
            error,section_end);
        goto fixoffset;

    case DW_FORM_ref_udata:
        {
        Dwarf_Byte_Ptr ptr = attr->ar_debug_ptr;
        Dwarf_Unsigned localoffset = 0;

        DECODE_LEB128_UWORD_CK(ptr,localoffset,
            dbg,error,section_end);
        offset = localoffset;

        fixoffset: /* we have a local offset, make it global */

        /* check legality of offset */
        if (offset >= cu_context->cc_length +
            cu_context->cc_length_size +
            cu_context->cc_extension_size) {
            _dwarf_error(dbg, error, DW_DLE_ATTR_FORM_OFFSET_BAD);
            return DW_DLV_ERROR;
        }

        /* globalize the offset */
        offset += cu_context->cc_debug_offset;
        }
        break;

    /*  The DWARF2 document did not make clear that
        DW_FORM_data4( and 8) were references with
        global offsets to some section.
        That was first clearly documented in DWARF3.
        In DWARF4 these two forms are no longer references. */
    case DW_FORM_data4:
        if (context_version >= DW_CU_VERSION4) {
            _dwarf_error(dbg, error, DW_DLE_NOT_REF_FORM);
            return DW_DLV_ERROR;
        }
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_32BIT_SIZE,
            error, section_end);
        /* The offset is global. */
        break;
    case DW_FORM_data8:
        if (context_version >= DW_CU_VERSION4) {
            _dwarf_error(dbg, error, DW_DLE_NOT_REF_FORM);
            return DW_DLV_ERROR;
        }
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_64BIT_SIZE,
            error,section_end);
        /* The offset is global. */
        break;
    case DW_FORM_ref_addr:
        {
            /*  In Dwarf V2 DW_FORM_ref_addr was defined
                as address-size even though it is a .debug_info
                offset.  Fixed in Dwarf V3 to be offset-size.
                */
            unsigned length_size = 0;
            if (context_version == 2) {
                length_size = cu_context->cc_address_size;
            } else {
                length_size = cu_context->cc_length_size;
            }
            if (length_size == 4) {
                READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
                    attr->ar_debug_ptr, DWARF_32BIT_SIZE,
                    error,section_end);
            } else if (length_size == 8) {
                READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
                    attr->ar_debug_ptr, DWARF_64BIT_SIZE,
                    error,section_end);
            } else {
                _dwarf_error(dbg, error,
                    DW_DLE_FORM_SEC_OFFSET_LENGTH_BAD);
                return DW_DLV_ERROR;
            }
        }
        break;
    /*  Index into .debug_rnglists/.debug_loclists section.
        Return the index itself. */
    case DW_FORM_loclistx:
    case DW_FORM_rnglistx: {
        unsigned length_size = cu_context->cc_length_size;
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            attr->ar_debug_ptr, length_size,
            error,section_end);
        }
        break;
    case DW_FORM_sec_offset:
    case DW_FORM_GNU_ref_alt:  /* 2013 GNU extension */
    case DW_FORM_GNU_strp_alt: /* 2013 GNU extension */
    case DW_FORM_strp_sup:     /* DWARF5, sup string section */
    case DW_FORM_line_strp:    /* DWARF5, .debug_line_str section */
        {
            /*  DW_FORM_sec_offset first exists in DWARF4.*/
            /*  It is up to the caller to know what the offset
                of DW_FORM_sec_offset, DW_FORM_strp_sup
                or DW_FORM_GNU_strp_alt etc refer to,
                the offset is not going to refer to .debug_info! */
            unsigned length_size = cu_context->cc_length_size;
            if (length_size == 4) {
                READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
                    attr->ar_debug_ptr, DWARF_32BIT_SIZE,
                    error,section_end);
            } else if (length_size == 8) {
                READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
                    attr->ar_debug_ptr, DWARF_64BIT_SIZE,
                    error,section_end);
            } else {
                _dwarf_error(dbg, error,
                    DW_DLE_FORM_SEC_OFFSET_LENGTH_BAD);
                return DW_DLV_ERROR;
            }
        }
        break;
    case DW_FORM_ref_sig8: {
        /*  This, in DWARF4, is how
            .debug_info refers to .debug_types. */
        Dwarf_Sig8 sig8;
        Dwarf_Bool t_is_info = TRUE;
        Dwarf_Unsigned t_offset = 0;

        if ((attr->ar_debug_ptr + sizeof(Dwarf_Sig8)) > section_end) {
            _dwarf_error_string(dbg, error,
                DW_DLE_REF_SIG8_NOT_HANDLED,
                "DW_DLE_REF_SIG8_NOT_HANDLED: "
                " Dwarf_Sig8 content runs off the end of its section");
            return DW_DLV_ERROR;
        }
        memcpy(&sig8,attr->ar_debug_ptr,sizeof(Dwarf_Sig8));
        res = find_sig8_target_as_global_offset(attr,
            &sig8,&t_is_info,&t_offset,error);
        if (res == DW_DLV_ERROR) {
            _dwarf_error_string(dbg, error,
                DW_DLE_REF_SIG8_NOT_HANDLED,
                "DW_DLE_REF_SIG8_NOT_HANDLED: "
                " problem finding target");
            return DW_DLV_ERROR;
        }
        if (res == DW_DLV_NO_ENTRY) {
            return res;
        }
        is_info = t_is_info;
        offset = t_offset;
        break;
    }
    default: {
        dwarfstring m;
        int formcode = attr->ar_attribute_form;
        int fcres = 0;
        const char *name = 0;

        dwarfstring_constructor(&m);
        dwarfstring_append_printf_u(&m,
            "DW_DLE_BAD_REF_FORM: The form code is 0x%x ",
            formcode);
        fcres  = dwarf_get_FORM_name (formcode,&name);
        if (fcres != DW_DLV_OK) {
            name="<UnknownFormCode>";
        }
        dwarfstring_append_printf_s(&m,
            " %s.",(char *)name);
        _dwarf_error_string(dbg, error, DW_DLE_BAD_REF_FORM,
            dwarfstring_string(&m));
        dwarfstring_destructor(&m);
        return DW_DLV_ERROR;
        }
    }

    *offset_is_info = is_info;
    *ret_offset = offset;
    return DW_DLV_OK;
}