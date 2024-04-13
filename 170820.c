dwarf_formsdata(Dwarf_Attribute attr,
    Dwarf_Signed * return_sval, Dwarf_Error * error)
{
    Dwarf_Signed ret_value = 0;
    Dwarf_Debug dbg = 0;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Byte_Ptr section_end = 0;

    int res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    section_end =
        _dwarf_calculate_info_section_end_ptr(cu_context);
    switch (attr->ar_attribute_form) {

    case DW_FORM_data1:
        if ( attr->ar_debug_ptr >= section_end) {
            _dwarf_error(dbg, error, DW_DLE_DIE_BAD);
            return DW_DLV_ERROR;
        }
        *return_sval = (*(Dwarf_Sbyte *) attr->ar_debug_ptr);
        return DW_DLV_OK;

    /*  READ_UNALIGNED does not sign extend.
        So we have to use a cast to get the
        value sign extended in the right way for each case. */
    case DW_FORM_data2:{
        READ_UNALIGNED_CK(dbg, ret_value, Dwarf_Signed,
            attr->ar_debug_ptr,
            DWARF_HALF_SIZE,
            error,section_end);
        *return_sval = (Dwarf_Shalf) ret_value;
        return DW_DLV_OK;

        }

    case DW_FORM_data4:{
        READ_UNALIGNED_CK(dbg, ret_value, Dwarf_Signed,
            attr->ar_debug_ptr,
            DWARF_32BIT_SIZE,
            error,section_end);
        SIGN_EXTEND(ret_value,DWARF_32BIT_SIZE);
        *return_sval = (Dwarf_Signed) ret_value;
        return DW_DLV_OK;
        }

    case DW_FORM_data8:{
        READ_UNALIGNED_CK(dbg, ret_value, Dwarf_Signed,
            attr->ar_debug_ptr,
            DWARF_64BIT_SIZE,
            error,section_end);
        /* No SIGN_EXTEND needed, we are filling all bytes already.*/
        *return_sval = (Dwarf_Signed) ret_value;
        return DW_DLV_OK;
        }

    /*  DW_FORM_implicit_const  is a value in the
        abbreviations, not in the DIEs. */
    case DW_FORM_implicit_const:
        *return_sval = attr->ar_implicit_const;
        return DW_DLV_OK;

    case DW_FORM_sdata: {
        Dwarf_Byte_Ptr tmp = attr->ar_debug_ptr;

        DECODE_LEB128_SWORD_CK(tmp, ret_value,
            dbg,error,section_end);
        *return_sval = ret_value;
        return DW_DLV_OK;

    }

        /* IRIX bug 583450. We do not allow reading sdata from a udata
            value. Caller can retry, calling udata */

    default:
        break;
    }
    generate_form_error(dbg,error,attr->ar_attribute_form,
        DW_DLE_ATTR_FORM_BAD,
        "DW_DLE_ATTR_FORM_BAD",
        "dwarf_formsdata");
    return DW_DLV_ERROR;
}