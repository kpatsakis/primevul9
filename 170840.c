dwarf_formblock(Dwarf_Attribute attr,
    Dwarf_Block ** return_block, Dwarf_Error * error)
{
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Debug dbg = 0;
    Dwarf_Block local_block;
    Dwarf_Block *out_block = 0;
    int res = 0;

    memset(&local_block,0,sizeof(local_block));
    res  = get_attr_dbg(&dbg,&cu_context,attr,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    res = _dwarf_formblock_internal(dbg,attr,
        cu_context, &local_block, error);
    if (res != DW_DLV_OK) {
        return res;
    }
    out_block = (Dwarf_Block *)
        _dwarf_get_alloc(dbg, DW_DLA_BLOCK, 1);
    if (!out_block) {
        _dwarf_error(dbg, error, DW_DLE_ALLOC_FAIL);
        return DW_DLV_ERROR;
    }
    *out_block = local_block;
    *return_block = out_block;
    return DW_DLV_OK;
}