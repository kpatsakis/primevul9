dwarf_formsig8(Dwarf_Attribute attr,
    Dwarf_Sig8 * returned_sig_bytes,
    Dwarf_Error* error)
{
    int res  = _dwarf_formsig8_internal(attr, DW_FORM_ref_sig8,
        returned_sig_bytes,error);
    return res;
}