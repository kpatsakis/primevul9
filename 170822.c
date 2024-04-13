dwarf_global_formref(Dwarf_Attribute attr,
    Dwarf_Off * ret_offset,
    Dwarf_Error * error)
{
    Dwarf_Bool is_info = 0;
    int res = 0;

    res = dwarf_global_formref_b(attr,ret_offset,
        &is_info,error);
    return res;
}