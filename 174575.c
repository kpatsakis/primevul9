compile_fill_jump_to_end(endlabel_T **el, int jump_where, cctx_T *cctx)
{
    garray_T	*instr = &cctx->ctx_instr;

    while (*el != NULL)
    {
	endlabel_T  *cur = (*el);
	isn_T	    *isn;

	isn = ((isn_T *)instr->ga_data) + cur->el_end_label;
	isn->isn_arg.jump.jump_where = jump_where;
	*el = cur->el_next;
	vim_free(cur);
    }
}