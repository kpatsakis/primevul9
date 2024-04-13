_dwarf_extract_data16(Dwarf_Debug dbg,
    Dwarf_Small *data,
    Dwarf_Small *section_start,
    Dwarf_Small *section_end,
    Dwarf_Form_Data16  * returned_val,
    Dwarf_Error *error)
{
    Dwarf_Small *data16end = 0;

    data16end = data + sizeof(Dwarf_Form_Data16);
    if (data  < section_start ||
        section_end < data16end) {
        _dwarf_error(dbg, error,DW_DLE_DATA16_OUTSIDE_SECTION);
        return DW_DLV_ERROR;
    }
    memcpy(returned_val, data, sizeof(*returned_val));
    return DW_DLV_OK;

}