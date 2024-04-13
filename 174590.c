compile_unlet(
    lval_T  *lvp,
    char_u  *name_end,
    exarg_T *eap,
    int	    deep UNUSED,
    void    *coookie)
{
    cctx_T	*cctx = coookie;
    char_u	*p = lvp->ll_name;
    int		cc = *name_end;
    int		ret = OK;

    if (cctx->ctx_skip == SKIP_YES)
	return OK;

    *name_end = NUL;
    if (*p == '$')
    {
	// :unlet $ENV_VAR
	ret = generate_UNLET(cctx, ISN_UNLETENV, p + 1, eap->forceit);
    }
    else if (vim_strchr(p, '.') != NULL || vim_strchr(p, '[') != NULL)
    {
	lhs_T	    lhs;

	// This is similar to assigning: lookup the list/dict, compile the
	// idx/key.  Then instead of storing the value unlet the item.
	// unlet {list}[idx]
	// unlet {dict}[key]  dict.key
	//
	// Figure out the LHS type and other properties.
	//
	ret = compile_lhs(p, &lhs, CMD_unlet, FALSE, FALSE, 0, cctx);

	// Use the info in "lhs" to unlet the item at the index in the
	// list or dict.
	if (ret == OK)
	{
	    if (!lhs.lhs_has_index)
	    {
		semsg(_(e_cannot_unlet_imported_item_str), p);
		ret = FAIL;
	    }
	    else
		ret = compile_assign_unlet(p, &lhs, FALSE, &t_void, cctx);
	}

	vim_free(lhs.lhs_name);
    }
    else if (check_vim9_unlet(p) == FAIL)
    {
	ret = FAIL;
    }
    else
    {
	// Normal name.  Only supports g:, w:, t: and b: namespaces.
	ret = generate_UNLET(cctx, ISN_UNLET, p, eap->forceit);
    }

    *name_end = cc;
    return ret;
}