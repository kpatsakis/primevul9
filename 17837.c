typval_tostring(typval_T *arg, int quotes)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*ret = NULL;

    if (arg == NULL)
	return vim_strsave((char_u *)"(does not exist)");
    if (!quotes && arg->v_type == VAR_STRING)
    {
	ret = vim_strsave(arg->vval.v_string == NULL ? (char_u *)""
							 : arg->vval.v_string);
    }
    else
    {
	ret = tv2string(arg, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	if (ret != NULL && tofree == NULL)
	    ret = vim_strsave(ret);
    }
    return ret;
}