option_was_set(char_u *name)
{
    int idx;

    idx = findoption(name);
    if (idx < 0)	/* unknown option */
	return FALSE;
    if (options[idx].flags & P_WAS_SET)
	return TRUE;
    return FALSE;
}