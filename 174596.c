compile_jump_to_end(endlabel_T **el, jumpwhen_T when, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;
    endlabel_T  *endlabel = ALLOC_CLEAR_ONE(endlabel_T);

    if (endlabel == NULL)
	return FAIL;
    endlabel->el_next = *el;
    *el = endlabel;
    endlabel->el_end_label = instr->ga_len;

    generate_JUMP(cctx, when, 0);
    return OK;
}