dwarf_hasform(Dwarf_Attribute attr,
    Dwarf_Half form,
    Dwarf_Bool * return_bool, Dwarf_Error * error)
{
    Dwarf_Debug dbg = 0;
    Dwarf_CU_Context cu_context = 0;

    int res  =get_attr_dbg(&dbg,&cu_context, attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    *return_bool = (attr->ar_attribute_form == form);
    return DW_DLV_OK;
}