eval_interp_string(char_u **arg, typval_T *rettv, int evaluate)
{
    typval_T	tv;
    int		ret = OK;
    int		quote;
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    for (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	if (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	else
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	if (ret == FAIL)
	    break;
	if (evaluate)
	{
	    ga_concat(&ga, tv.vval.v_string);
	    clear_tv(&tv);
	}

	if (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    break;
	}
	p = eval_one_expr_in_str(*arg, &ga, evaluate);
	if (p == NULL)
	{
	    ret = FAIL;
	    break;
	}
	*arg = p;
    }

    rettv->v_type = VAR_STRING;
    if (ret == FAIL || !evaluate || ga_append(&ga, NUL) == FAIL)
    {
	ga_clear(&ga);
	rettv->vval.v_string = NULL;
	return ret;
    }

    rettv->vval.v_string = ga.ga_data;
    return OK;
}