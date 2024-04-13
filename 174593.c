compile_script(char_u *line, cctx_T *cctx)
{
    if (cctx->ctx_skip != SKIP_YES)
    {
	isn_T	*isn;

	if ((isn = generate_instr(cctx, ISN_EXEC_SPLIT)) == NULL)
	    return NULL;
	isn->isn_arg.string = vim_strsave(line);
    }
    return (char_u *)"";
}