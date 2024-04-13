del_termcode(char_u *name)
{
    int	    i;

    if (termcodes == NULL)	// nothing there yet
	return;

    need_gather = TRUE;		// need to fill termleader[]

    for (i = 0; i < tc_len; ++i)
	if (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	{
	    del_termcode_idx(i);
	    return;
	}
    // not found. Give error message?
}