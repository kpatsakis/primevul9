eval_env_var(char_u **arg, typval_T *rettv, int evaluate)
{
    char_u	*string = NULL;
    int		len;
    int		cc;
    char_u	*name;
    int		mustfree = FALSE;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    if (evaluate)
    {
	if (len == 0)
	    return FAIL; // invalid empty name

	cc = name[len];
	name[len] = NUL;
	// first try vim_getenv(), fast for normal environment vars
	string = vim_getenv(name, &mustfree);
	if (string != NULL && *string != NUL)
	{
	    if (!mustfree)
		string = vim_strsave(string);
	}
	else
	{
	    if (mustfree)
		vim_free(string);

	    // next try expanding things like $VIM and ${HOME}
	    string = expand_env_save(name - 1);
	    if (string != NULL && *string == '$')
		VIM_CLEAR(string);
	}
	name[len] = cc;

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = string;
	rettv->v_lock = 0;
    }

    return OK;
}