_dwarf_formudata_internal(Dwarf_Debug dbg,
    Dwarf_Attribute attr,
    unsigned form,
    Dwarf_Byte_Ptr data,
    Dwarf_Byte_Ptr section_end,
    Dwarf_Unsigned *return_uval,
    Dwarf_Unsigned *bytes_read,
    Dwarf_Error *error)
{
    Dwarf_Unsigned ret_value = 0;

    switch (form) {
    case DW_FORM_data1:
        READ_UNALIGNED_CK(dbg, ret_value, Dwarf_Unsigned,
            data, sizeof(Dwarf_Small),
            error,section_end);
        *return_uval = ret_value;
        *bytes_read = 1;
        return DW_DLV_OK;

    /*  READ_UNALIGNED does the right thing as it reads
        the right number bits and generates host order.
        So we can just assign to *return_uval. */
    case DW_FORM_data2:{
        READ_UNALIGNED_CK(dbg, ret_value, Dwarf_Unsigned,
            data, DWARF_HALF_SIZE,
            error,section_end);
        *return_uval = ret_value;
        *bytes_read = DWARF_HALF_SIZE;
        return DW_DLV_OK;
        }

    case DW_FORM_data4:{
        READ_UNALIGNED_CK(dbg, ret_value, Dwarf_Unsigned,
            data,
            DWARF_32BIT_SIZE,
            error,section_end);
        *return_uval = ret_value;
        *bytes_read = DWARF_32BIT_SIZE;;
        return DW_DLV_OK;
        }

    case DW_FORM_data8:{
        READ_UNALIGNED_CK(dbg, ret_value, Dwarf_Unsigned,
            data,
            DWARF_64BIT_SIZE,
            error,section_end);
        *return_uval = ret_value;
        *bytes_read = DWARF_64BIT_SIZE;
        return DW_DLV_OK;
        }
        break;
    /* real udata */
    case DW_FORM_loclistx:
    case DW_FORM_rnglistx:
    case DW_FORM_udata: {
        Dwarf_Unsigned leblen = 0;
        DECODE_LEB128_UWORD_LEN_CK(data, ret_value,leblen,
            dbg,error,section_end);
        *return_uval = ret_value;
        *bytes_read = leblen;
        return DW_DLV_OK;
    }
    /*  IRIX bug 583450. We do not allow reading
        sdata from a udata
        value. Caller can retry, calling sdata */
    default:
        break;
    }
    if (attr) {
        int res = 0;
        Dwarf_Signed s = 0;
        res = dwarf_formsdata(attr,&s,error);
        if (res != DW_DLV_OK) {
            return res;
        }
        if (s < 0 ) {
            _dwarf_error(dbg, error, DW_DLE_UDATA_VALUE_NEGATIVE);
            return DW_DLV_ERROR;
        }
        *return_uval = (Dwarf_Unsigned)s;
        *bytes_read = 0;
        return DW_DLV_OK;
    }
    generate_form_error(dbg,error,form,
        DW_DLE_ATTR_FORM_BAD,
        "DW_DLE_ATTR_FORM_BAD",
        "formudata (internal function)");
    return DW_DLV_ERROR;
}