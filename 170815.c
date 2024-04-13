_dwarf_formblock_internal(Dwarf_Debug dbg,
    Dwarf_Attribute attr,
    Dwarf_CU_Context cu_context,
    Dwarf_Block * return_block,
    Dwarf_Error * error)
{
    Dwarf_Small *section_start = 0;
    Dwarf_Small *section_end = 0;
    Dwarf_Unsigned section_length = 0;
    Dwarf_Unsigned length = 0;
    Dwarf_Small *data = 0;

    section_end =
        _dwarf_calculate_info_section_end_ptr(cu_context);
    section_start =
        _dwarf_calculate_info_section_start_ptr(cu_context,
        &section_length);

    switch (attr->ar_attribute_form) {

    case DW_FORM_block1:
        length = *(Dwarf_Small *) attr->ar_debug_ptr;
        data = attr->ar_debug_ptr + sizeof(Dwarf_Small);
        break;

    case DW_FORM_block2:
        READ_UNALIGNED_CK(dbg, length, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_HALF_SIZE,
            error,section_end);
        data = attr->ar_debug_ptr + DWARF_HALF_SIZE;
        break;

    case DW_FORM_block4:
        READ_UNALIGNED_CK(dbg, length, Dwarf_Unsigned,
            attr->ar_debug_ptr, DWARF_32BIT_SIZE,
            error,section_end);
        data = attr->ar_debug_ptr + DWARF_32BIT_SIZE;
        break;

    case DW_FORM_exprloc:
    case DW_FORM_block: {
        Dwarf_Byte_Ptr tmp = attr->ar_debug_ptr;
        Dwarf_Unsigned leblen = 0;

        DECODE_LEB128_UWORD_LEN_CK(tmp, length, leblen,
            dbg,error,section_end);
        data = attr->ar_debug_ptr + leblen;
        break;
        }
    default:
        generate_form_error(dbg,error,attr->ar_attribute_form,
            DW_DLE_ATTR_FORM_BAD,
            "DW_DLE_ATTR_FORM_BAD",
            "dwarf_formblock");
        return DW_DLV_ERROR;
    }
    /*  We have the data. Check for errors. */
    if (length >= section_length) {
        /*  Sanity test looking for wraparound:
            when length actually added in
            it would not be caught.
            Test could be just >, but >= ok here too.*/
        _dwarf_error_string(dbg, error,
            DW_DLE_FORM_BLOCK_LENGTH_ERROR,
            "DW_DLE_FORM_BLOCK_LENGTH_ERROR: "
            "The length of the block is greater "
            "than the section length! Corrupt Dwarf.");
        return DW_DLV_ERROR;
    }
    if ((attr->ar_debug_ptr + length) > section_end) {
        _dwarf_error_string(dbg, error,
            DW_DLE_FORM_BLOCK_LENGTH_ERROR,
            "DW_DLE_FORM_BLOCK_LENGTH_ERROR: "
            "The block length means the block "
            "runs off the end of the section length!"
            " Corrupt Dwarf.");
        return DW_DLV_ERROR;
    }
    if (data > section_end) {
        _dwarf_error_string(dbg, error,
            DW_DLE_FORM_BLOCK_LENGTH_ERROR,
            "DW_DLE_FORM_BLOCK_LENGTH_ERROR: "
            "The block content is "
            "past the end of the section!"
            " Corrupt Dwarf.");
        _dwarf_error(dbg, error, DW_DLE_FORM_BLOCK_LENGTH_ERROR);
        return DW_DLV_ERROR;
    }
    if ((data + length) > section_end) {
        _dwarf_error_string(dbg, error,
            DW_DLE_FORM_BLOCK_LENGTH_ERROR,
            "DW_DLE_FORM_BLOCK_LENGTH_ERROR: "
            "The end of the block content is "
            "past the end of the section!"
            " Corrupt Dwarf.");
        return DW_DLV_ERROR;
    }
    return_block->bl_len = length;
    return_block->bl_data = data;
    /*  This struct is public so use the old name instead
        of what we now would call it:  bl_kind  */
    return_block->bl_from_loclist =  DW_LKIND_expression;
    return_block->bl_section_offset =  data - section_start;
    return DW_DLV_OK;
}