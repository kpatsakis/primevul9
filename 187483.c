fex_format(
    linenr_T	lnum,
    long	count,
    int		c)	// character to be inserted
{
    int		use_sandbox = was_set_insecurely((char_u *)"formatexpr",
								   OPT_LOCAL);
    int		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_COUNT, count);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    if (fex == NULL)
	return 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    if (use_sandbox)
	++sandbox;
    r = (int)eval_to_number(fex);
    if (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, NULL, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    return r;
}