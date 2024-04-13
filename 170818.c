dwarf_formstring(Dwarf_Attribute attr,
    char **return_str, Dwarf_Error * error)
{
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Debug dbg = 0;
    Dwarf_Unsigned offset = 0;
    int res = DW_DLV_ERROR;
    Dwarf_Small *secdataptr = 0;
    Dwarf_Small *secend = 0;
    Dwarf_Unsigned secdatalen = 0;
    Dwarf_Small *infoptr = attr->ar_debug_ptr;
    Dwarf_Small *contextend = 0;

    res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    if (cu_context->cc_is_info) {
        secdataptr = (Dwarf_Small *)dbg->de_debug_info.dss_data;
        secdatalen = dbg->de_debug_info.dss_size;
    } else {
        secdataptr = (Dwarf_Small *)dbg->de_debug_types.dss_data;
        secdatalen = dbg->de_debug_types.dss_size;
    }
    contextend = secdataptr +
        cu_context->cc_debug_offset +
        cu_context->cc_length +
        cu_context->cc_length_size +
        cu_context->cc_extension_size;
    secend = secdataptr + secdatalen;
    if (contextend < secend) {
        secend = contextend;
    }
    switch(attr->ar_attribute_form) {
    case DW_FORM_string: {
        Dwarf_Small *begin = attr->ar_debug_ptr;

        res= _dwarf_check_string_valid(dbg,secdataptr,begin, secend,
            DW_DLE_FORM_STRING_BAD_STRING,error);
        if (res != DW_DLV_OK) {
            return res;
        }
        *return_str = (char *) (begin);
        return DW_DLV_OK;
    }
    case DW_FORM_GNU_strp_alt:
    case DW_FORM_strp_sup:  {
        Dwarf_Error alterr = 0;
        Dwarf_Bool is_info = TRUE;
        /*  See dwarfstd.org issue 120604.1
            This is the offset in the .debug_str section
            of another object file.
            The 'tied' file notion should apply.
            It is not clear whether both a supplementary
            and a split object might be needed at the same time
            (hence two 'tied' files simultaneously). */
        Dwarf_Off soffset = 0;

        res = dwarf_global_formref_b(attr, &soffset,
            &is_info,error);
        if (res != DW_DLV_OK) {
            return res;
        }
        res = _dwarf_get_string_from_tied(dbg, soffset,
            return_str, &alterr);
        if (res == DW_DLV_ERROR) {
            if (dwarf_errno(alterr) ==
                DW_DLE_NO_TIED_FILE_AVAILABLE) {
                dwarf_dealloc(dbg,alterr,DW_DLA_ERROR);
                if ( attr->ar_attribute_form ==
                    DW_FORM_GNU_strp_alt) {
                    *return_str =
                        (char *)"<DW_FORM_GNU_strp_alt-no-tied-file>";
                } else {
                    *return_str =
                        (char *)"<DW_FORM_strp_sup-no-tied-file>";
                }
                return DW_DLV_OK;
            }
            if (error) {
                *error = alterr;
            } else {
                dwarf_dealloc_error(dbg,alterr);
                alterr = 0;
            }
            return res;
        }
        if (res == DW_DLV_NO_ENTRY) {
            if ( attr->ar_attribute_form == DW_FORM_GNU_strp_alt) {
                *return_str =
                    (char *)"<DW_FORM_GNU_strp_alt-no-tied-file>";
            }else {
                *return_str =
                    (char *)"<DW_FORM_strp_sup-no-tied-file>";
            }
        }
        return res;
    }
    case DW_FORM_GNU_str_index:
    case DW_FORM_strx:
    case DW_FORM_strx1:
    case DW_FORM_strx2:
    case DW_FORM_strx3:
    case DW_FORM_strx4: {
        Dwarf_Unsigned offsettostr= 0;

        res = _dwarf_extract_string_offset_via_str_offsets(dbg,
            infoptr,
            secend,
            attr->ar_attribute,
            attr->ar_attribute_form,
            cu_context,
            &offsettostr,
            error);
        if (res != DW_DLV_OK) {
            return res;
        }
        offset = offsettostr;
        break;
    }
    case DW_FORM_strp:
    case DW_FORM_line_strp:{
        READ_UNALIGNED_CK(dbg, offset, Dwarf_Unsigned,
            infoptr,
            cu_context->cc_length_size,error,secend);
        break;
    }
    default:
        _dwarf_error(dbg, error, DW_DLE_STRING_FORM_IMPROPER);
        return DW_DLV_ERROR;
    }
    /*  Now we have offset so read the string from
        debug_str or debug_line_str. */
    res = _dwarf_extract_local_debug_str_string_given_offset(dbg,
        attr->ar_attribute_form,
        offset,
        return_str,
        error);
    return res;
}