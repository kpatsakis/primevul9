dwarf_uncompress_integer_block_a(Dwarf_Debug dbg,
    Dwarf_Unsigned     input_length_in_bytes,
    void             * input_block,
    Dwarf_Unsigned   * value_count,
    Dwarf_Signed    ** value_array,
    Dwarf_Error      * error)
{
    Dwarf_Unsigned output_length_in_units = 0;
    Dwarf_Signed * output_block = 0;
    unsigned i = 0;
    char * ptr = 0;
    int remain = 0;
    Dwarf_Signed * array = 0;
    Dwarf_Byte_Ptr endptr = (Dwarf_Byte_Ptr)input_block+
        input_length_in_bytes;

    output_length_in_units = 0;
    remain = input_length_in_bytes;
    ptr = input_block;
    while (remain > 0) {
        Dwarf_Unsigned len = 0;
        Dwarf_Signed value = 0;
        int rres = 0;

        rres = dwarf_decode_signed_leb128((char *)ptr,
            &len, &value,(char *)endptr);
        if (rres != DW_DLV_OK) {
            _dwarf_error(NULL, error, DW_DLE_LEB_IMPROPER);
            return DW_DLV_ERROR;
        }
        ptr += len;
        remain -= len;
        output_length_in_units++;
    }
    if (remain != 0) {
        _dwarf_error(NULL, error, DW_DLE_ALLOC_FAIL);
        return DW_DLV_ERROR;
    }

    output_block = (Dwarf_Signed*)
        _dwarf_get_alloc(dbg,
            DW_DLA_STRING,
            output_length_in_units * sizeof(Dwarf_Signed));
    if (!output_block) {
        _dwarf_error(dbg, error, DW_DLE_ALLOC_FAIL);
        return DW_DLV_ERROR;
    }
    array = output_block;
    remain = input_length_in_bytes;
    ptr = input_block;
    for (i=0; i<output_length_in_units && remain>0; i++) {
        Dwarf_Signed num;
        Dwarf_Unsigned len;
        int sres = 0;

        sres = dwarf_decode_signed_leb128((char *)ptr,
            &len, &num,(char *)endptr);
        if (sres != DW_DLV_OK) {
            dwarf_dealloc(dbg,output_block,DW_DLA_STRING);
            _dwarf_error(NULL, error, DW_DLE_LEB_IMPROPER);
            return DW_DLV_ERROR;
        }
        ptr += len;
        remain -= len;
        array[i] = num;
    }

    if (remain != 0) {
        dwarf_dealloc(dbg, (unsigned char *)output_block,
            DW_DLA_STRING);
        _dwarf_error(dbg, error, DW_DLE_ALLOC_FAIL);
        return DW_DLV_ERROR;
    }

    *value_count = output_length_in_units;
    *value_array = output_block;
    return DW_DLV_OK;
}