S_alloc_code_blocks(pTHX_  int ncode)
{
     struct reg_code_blocks *cbs;
    Newx(cbs, 1, struct reg_code_blocks);
    cbs->count = ncode;
    cbs->refcnt = 1;
    SAVEDESTRUCTOR_X(S_free_codeblocks, cbs);
    if (ncode)
        Newx(cbs->cb, ncode, struct reg_code_block);
    else
        cbs->cb = NULL;
    return cbs;
}