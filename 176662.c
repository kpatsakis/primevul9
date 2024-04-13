eval7_leader(
	typval_T    *rettv,
	int	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    int		ret = OK;
    int		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    int		vim9script = in_vim9script();
#ifdef FEAT_FLOAT
    float_T	    f = 0.0;

    if (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    else
#endif
    {
	while (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	if (vim9script && end_leader[-1] == '!')
	    val = tv2bool(rettv);
	else
	    val = tv_get_number_chk(rettv, &error);
    }
    if (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    else
    {
	while (end_leader > start_leader)
	{
	    --end_leader;
	    if (*end_leader == '!')
	    {
		if (numeric_only)
		{
		    ++end_leader;
		    break;
		}
#ifdef FEAT_FLOAT
		if (rettv->v_type == VAR_FLOAT)
		{
		    if (vim9script)
		    {
			rettv->v_type = VAR_BOOL;
			val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    }
		    else
			f = !f;
		}
		else
#endif
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    else if (*end_leader == '-')
	    {
#ifdef FEAT_FLOAT
		if (rettv->v_type == VAR_FLOAT)
		    f = -f;
		else
#endif
		{
		    val = -val;
		    type = VAR_NUMBER;
		}
	    }
	}
#ifdef FEAT_FLOAT
	if (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	else
#endif
	{
	    clear_tv(rettv);
	    if (vim9script)
		rettv->v_type = type;
	    else
		rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = val;
	}
    }
    *end_leaderp = end_leader;
    return ret;
}