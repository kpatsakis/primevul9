tv_equal(
    typval_T *tv1,
    typval_T *tv2,
    int	     ic,	    // ignore case
    int	     recursive)	    // TRUE when used recursively
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1, *s2;
    static int  recursive_cnt = 0;	    // catch recursive loops
    int		r;
    static int	tv_equal_recurse_limit;

    // Catch lists and dicts that have an endless loop by limiting
    // recursiveness to a limit.  We guess they are equal then.
    // A fixed limit has the problem of still taking an awful long time.
    // Reduce the limit every time running into it. That should work fine for
    // deeply linked structures that are not recursively linked and catch
    // recursiveness quickly.
    if (!recursive)
	tv_equal_recurse_limit = 1000;
    if (recursive_cnt >= tv_equal_recurse_limit)
    {
	--tv_equal_recurse_limit;
	return TRUE;
    }

    // For VAR_FUNC and VAR_PARTIAL compare the function name, bound dict and
    // arguments.
    if ((tv1->v_type == VAR_FUNC
		|| (tv1->v_type == VAR_PARTIAL && tv1->vval.v_partial != NULL))
	    && (tv2->v_type == VAR_FUNC
		|| (tv2->v_type == VAR_PARTIAL && tv2->vval.v_partial != NULL)))
    {
	++recursive_cnt;
	r = func_equal(tv1, tv2, ic);
	--recursive_cnt;
	return r;
    }

    if (tv1->v_type != tv2->v_type
	    && ((tv1->v_type != VAR_BOOL && tv1->v_type != VAR_SPECIAL)
		|| (tv2->v_type != VAR_BOOL && tv2->v_type != VAR_SPECIAL)))
	return FALSE;

    switch (tv1->v_type)
    {
	case VAR_LIST:
	    ++recursive_cnt;
	    r = list_equal(tv1->vval.v_list, tv2->vval.v_list, ic, TRUE);
	    --recursive_cnt;
	    return r;

	case VAR_DICT:
	    ++recursive_cnt;
	    r = dict_equal(tv1->vval.v_dict, tv2->vval.v_dict, ic, TRUE);
	    --recursive_cnt;
	    return r;

	case VAR_BLOB:
	    return blob_equal(tv1->vval.v_blob, tv2->vval.v_blob);

	case VAR_NUMBER:
	case VAR_BOOL:
	case VAR_SPECIAL:
	    return tv1->vval.v_number == tv2->vval.v_number;

	case VAR_STRING:
	    s1 = tv_get_string_buf(tv1, buf1);
	    s2 = tv_get_string_buf(tv2, buf2);
	    return ((ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2)) == 0);

	case VAR_FLOAT:
#ifdef FEAT_FLOAT
	    return tv1->vval.v_float == tv2->vval.v_float;
#endif
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    return tv1->vval.v_job == tv2->vval.v_job;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    return tv1->vval.v_channel == tv2->vval.v_channel;
#endif
	case VAR_INSTR:
	    return tv1->vval.v_instr == tv2->vval.v_instr;

	case VAR_PARTIAL:
	    return tv1->vval.v_partial == tv2->vval.v_partial;

	case VAR_FUNC:
	    return tv1->vval.v_string == tv2->vval.v_string;

	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	    break;
    }

    // VAR_UNKNOWN can be the result of a invalid expression, let's say it
    // does not equal anything, not even itself.
    return FALSE;
}