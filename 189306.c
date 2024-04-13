find_mps_values(
    int	    *initc,
    int	    *findc,
    int	    *backwards,
    int	    switchit)
{
    char_u	*ptr;

    ptr = curbuf->b_p_mps;
    while (*ptr != NUL)
    {
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    char_u *prev;

	    if (mb_ptr2char(ptr) == *initc)
	    {
		if (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = TRUE;
		}
		else
		{
		    *findc = mb_ptr2char(ptr + mb_ptr2len(ptr) + 1);
		    *backwards = FALSE;
		}
		return;
	    }
	    prev = ptr;
	    ptr += mb_ptr2len(ptr) + 1;
	    if (mb_ptr2char(ptr) == *initc)
	    {
		if (switchit)
		{
		    *findc = *initc;
		    *initc = mb_ptr2char(prev);
		    *backwards = FALSE;
		}
		else
		{
		    *findc = mb_ptr2char(prev);
		    *backwards = TRUE;
		}
		return;
	    }
	    ptr += mb_ptr2len(ptr);
	}
	else
#endif
	{
	    if (*ptr == *initc)
	    {
		if (switchit)
		{
		    *backwards = TRUE;
		    *findc = *initc;
		    *initc = ptr[2];
		}
		else
		{
		    *backwards = FALSE;
		    *findc = ptr[2];
		}
		return;
	    }
	    ptr += 2;
	    if (*ptr == *initc)
	    {
		if (switchit)
		{
		    *backwards = FALSE;
		    *findc = *initc;
		    *initc = ptr[-2];
		}
		else
		{
		    *backwards = TRUE;
		    *findc =  ptr[-2];
		}
		return;
	    }
	    ++ptr;
	}
	if (*ptr == ',')
	    ++ptr;
    }
}