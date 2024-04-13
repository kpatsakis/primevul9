reg_submatch(int no)
{
    char_u	*retval = NULL;
    char_u	*s;
    int		len;
    int		round;
    linenr_T	lnum;

    if (!can_f_submatch || no < 0)
	return NULL;

    if (rsm.sm_match == NULL)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	for (round = 1; round <= 2; ++round)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    if (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		return NULL;

	    s = reg_getline_submatch(lnum);
	    if (s == NULL)  // anti-crash check, cannot happen?
		break;
	    s += rsm.sm_mmatch->startpos[no].col;
	    if (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].col
					  - rsm.sm_mmatch->startpos[no].col;
		if (round == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    else
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (int)STRLEN(s);
		if (round == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		while (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum++);
		    if (round == 2)
			STRCPY(retval + len, s);
		    len += (int)STRLEN(s);
		    if (round == 2)
			retval[len] = '\n';
		    ++len;
		}
		if (round == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].col);
		len += rsm.sm_mmatch->endpos[no].col;
		if (round == 2)
		    retval[len] = NUL;
		++len;
	    }

	    if (retval == NULL)
	    {
		retval = alloc(len);
		if (retval == NULL)
		    return NULL;
	    }
	}
    }
    else
    {
	s = rsm.sm_match->startp[no];
	if (s == NULL || rsm.sm_match->endp[no] == NULL)
	    retval = NULL;
	else
	    retval = vim_strnsave(s, rsm.sm_match->endp[no] - s);
    }

    return retval;
}