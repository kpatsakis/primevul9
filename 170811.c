dwarf_whatattr(Dwarf_Attribute attr,
    Dwarf_Half * return_attr, Dwarf_Error * error)
{
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Debug dbg = 0;

    int res  =get_attr_dbg(&dbg,&cu_context, attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    *return_attr = (attr->ar_attribute);
    return DW_DLV_OK;
}