buf_init_chartab(
    buf_T	*buf,
    int		global)		// FALSE: only set buf->b_chartab[]
{
    int		c;
    int		c2;
    char_u	*p;
    int		i;
    int		tilde;
    int		do_isalpha;

    if (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 */
	c = 0;
	while (c < ' ')
	    g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	while (c <= '~')
	    g_chartab[c++] = 1 + CT_PRINT_CHAR;
	while (c < 256)
	{
	    // UTF-8: bytes 0xa0 - 0xff are printable (latin1)
	    if (enc_utf8 && c >= 0xa0)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // euc-jp characters starting with 0x8e are single width
	    else if (enc_dbcs == DBCS_JPNU && c == 0x8e)
		g_chartab[c++] = CT_PRINT_CHAR + 1;
	    // other double-byte chars can be printable AND double-width
	    else if (enc_dbcs != 0 && MB_BYTE2LEN(c) == 2)
		g_chartab[c++] = CT_PRINT_CHAR + 2;
	    else
		// the rest is unprintable by default
		g_chartab[c++] = (dy_flags & DY_UHEX) ? 4 : 2;
	}

	// Assume that every multi-byte char is a filename character.
	for (c = 1; c < 256; ++c)
	    if ((enc_dbcs != 0 && MB_BYTE2LEN(c) > 1)
		    || (enc_dbcs == DBCS_JPNU && c == 0x8e)
		    || (enc_utf8 && c >= 0xa0))
		g_chartab[c] |= CT_FNAME_CHAR;
    }

    /*
     * Init word char flags all to FALSE
     */
    CLEAR_FIELD(buf->b_chartab);
    if (enc_dbcs != 0)
	for (c = 0; c < 256; ++c)
	{
	    // double-byte characters are probably word characters
	    if (MB_BYTE2LEN(c) == 2)
		SET_CHARTAB(buf, c);
	}

#ifdef FEAT_LISP
    /*
     * In lisp mode the '-' character is included in keywords.
     */
    if (buf->b_p_lisp)
	SET_CHARTAB(buf, '-');
#endif

    // Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint'
    // options Each option is a list of characters, character numbers or
    // ranges, separated by commas, e.g.: "200-210,x,#-178,-"
    for (i = global ? 0 : 3; i <= 3; ++i)
    {
	if (i == 0)
	    p = p_isi;		// first round: 'isident'
	else if (i == 1)
	    p = p_isp;		// second round: 'isprint'
	else if (i == 2)
	    p = p_isf;		// third round: 'isfname'
	else	// i == 3
	    p = buf->b_p_isk;	// fourth round: 'iskeyword'

	while (*p)
	{
	    tilde = FALSE;
	    do_isalpha = FALSE;
	    if (*p == '^' && p[1] != NUL)
	    {
		tilde = TRUE;
		++p;
	    }
	    if (VIM_ISDIGIT(*p))
		c = getdigits(&p);
	    else if (has_mbyte)
		c = mb_ptr2char_adv(&p);
	    else
		c = *p++;
	    c2 = -1;
	    if (*p == '-' && p[1] != NUL)
	    {
		++p;
		if (VIM_ISDIGIT(*p))
		    c2 = getdigits(&p);
		else if (has_mbyte)
		    c2 = mb_ptr2char_adv(&p);
		else
		    c2 = *p++;
	    }
	    if (c <= 0 || c >= 256 || (c2 < c && c2 != -1) || c2 >= 256
						 || !(*p == NUL || *p == ','))
		return FAIL;

	    if (c2 == -1)	// not a range
	    {
		/*
		 * A single '@' (not "@-@"):
		 * Decide on letters being ID/printable/keyword chars with
		 * standard function isalpha(). This takes care of locale for
		 * single-byte characters).
		 */
		if (c == '@')
		{
		    do_isalpha = TRUE;
		    c = 1;
		    c2 = 255;
		}
		else
		    c2 = c;
	    }
	    while (c <= c2)
	    {
		// Use the MB_ functions here, because isalpha() doesn't
		// work properly when 'encoding' is "latin1" and the locale is
		// "C".
		if (!do_isalpha || MB_ISLOWER(c) || MB_ISUPPER(c))
		{
		    if (i == 0)			// (re)set ID flag
		    {
			if (tilde)
			    g_chartab[c] &= ~CT_ID_CHAR;
			else
			    g_chartab[c] |= CT_ID_CHAR;
		    }
		    else if (i == 1)		// (re)set printable
		    {
			if ((c < ' ' || c > '~'
				// For double-byte we keep the cell width, so
				// that we can detect it from the first byte.
			    ) && !(enc_dbcs && MB_BYTE2LEN(c) == 2))
			{
			    if (tilde)
			    {
				g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK)
					     + ((dy_flags & DY_UHEX) ? 4 : 2);
				g_chartab[c] &= ~CT_PRINT_CHAR;
			    }
			    else
			    {
				g_chartab[c] = (g_chartab[c] & ~CT_CELL_MASK) + 1;
				g_chartab[c] |= CT_PRINT_CHAR;
			    }
			}
		    }
		    else if (i == 2)		// (re)set fname flag
		    {
			if (tilde)
			    g_chartab[c] &= ~CT_FNAME_CHAR;
			else
			    g_chartab[c] |= CT_FNAME_CHAR;
		    }
		    else // i == 3		 (re)set keyword flag
		    {
			if (tilde)
			    RESET_CHARTAB(buf, c);
			else
			    SET_CHARTAB(buf, c);
		    }
		}
		++c;
	    }

	    c = *p;
	    p = skip_to_option_part(p);
	    if (c == ',' && *p == NUL)
		// Trailing comma is not allowed.
		return FAIL;
	}
    }
    chartab_initialized = TRUE;
    return OK;
}