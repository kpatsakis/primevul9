dwarf_formsig8_const(Dwarf_Attribute attr,
    Dwarf_Sig8 * returned_sig_bytes,
    Dwarf_Error* error)
{
    int res  =_dwarf_formsig8_internal(attr, DW_FORM_data8,
        returned_sig_bytes,error);
    return res;
}