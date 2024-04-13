expand_wildcards_eval(
    char_u	 **pat,		// pointer to input pattern
    int		  *num_file,	// resulting number of files
    char_u	***file,	// array of resulting files
    int		   flags)	// EW_DIR, etc.
{
    int		ret = FAIL;
    char_u	*eval_pat = NULL;
    char_u	*exp_pat = *pat;
    char      *ignored_msg;
    int		usedlen;

    if (*exp_pat == '%' || *exp_pat == '#' || *exp_pat == '<')
    {
	++emsg_off;
	eval_pat = eval_vars(exp_pat, exp_pat, &usedlen,
						    NULL, &ignored_msg, NULL);
	--emsg_off;
	if (eval_pat != NULL)
	    exp_pat = concat_str(eval_pat, exp_pat + usedlen);
    }

    if (exp_pat != NULL)
	ret = expand_wildcards(1, &exp_pat, num_file, file, flags);

    if (eval_pat != NULL)
    {
	vim_free(exp_pat);
	vim_free(eval_pat);
    }

    return ret;
}