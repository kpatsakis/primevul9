vim_str2nr(
    char_u		*start,
    int			*prep,	    // return: type of number 0 = decimal, 'x'
				    // or 'X' is hex, '0', 'o' or 'O' is octal,
				    // 'b' or 'B' is bin
    int			*len,	    // return: detected length of number
    int			what,	    // what numbers to recognize
    varnumber_T		*nptr,	    // return: signed result
    uvarnumber_T	*unptr,	    // return: unsigned result
    int			maxlen,     // max length of string to check
    int			strict)     // check strictly
{
    char_u	    *ptr = start;
    int		    pre = 0;		// default is decimal
    int		    negative = FALSE;
    uvarnumber_T    un = 0;
    int		    n;

    if (len != NULL)
	*len = 0;

    if (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    // Recognize hex, octal, and bin.
    if (ptr[0] == '0' && ptr[1] != '8' && ptr[1] != '9'
					       && (maxlen == 0 || maxlen > 1))
    {
	pre = ptr[1];
	if ((what & STR2NR_HEX)
		&& (pre == 'X' || pre == 'x') && vim_isxdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // hexadecimal
	    ptr += 2;
	else if ((what & STR2NR_BIN)
		&& (pre == 'B' || pre == 'b') && vim_isbdigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // binary
	    ptr += 2;
	else if ((what & STR2NR_OOCT)
		&& (pre == 'O' || pre == 'o') && vim_isodigit(ptr[2])
		&& (maxlen == 0 || maxlen > 2))
	    // octal with prefix "0o"
	    ptr += 2;
	else
	{
	    // decimal or octal, default is decimal
	    pre = 0;
	    if (what & STR2NR_OCT)
	    {
		// Don't interpret "0", "08" or "0129" as octal.
		for (n = 1; n != maxlen && VIM_ISDIGIT(ptr[n]); ++n)
		{
		    if (ptr[n] > '7')
		    {
			pre = 0;	// can't be octal
			break;
		    }
		    pre = '0';	// assume octal
		}
	    }
	}
    }

    // Do the conversion manually to avoid sscanf() quirks.
    n = 1;
    if (pre == 'B' || pre == 'b'
			     || ((what & STR2NR_BIN) && (what & STR2NR_FORCE)))
    {
	// bin
	if (pre != 0)
	    n += 2;	    // skip over "0b"
	while ('0' <= *ptr && *ptr <= '1')
	{
	    // avoid ubsan error for overflow
	    if (un <= UVARNUM_MAX / 2)
		un = 2 * un + (uvarnumber_T)(*ptr - '0');
	    else
		un = UVARNUM_MAX;
	    ++ptr;
	    if (n++ == maxlen)
		break;
	    if ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '1')
	    {
		++ptr;
		if (n++ == maxlen)
		    break;
	    }
	}
    }
    else if (pre == 'O' || pre == 'o' ||
		pre == '0' || ((what & STR2NR_OCT) && (what & STR2NR_FORCE)))
    {
	// octal
	if (pre != 0 && pre != '0')
	    n += 2;	    // skip over "0o"
	while ('0' <= *ptr && *ptr <= '7')
	{
	    // avoid ubsan error for overflow
	    if (un <= UVARNUM_MAX / 8)
		un = 8 * un + (uvarnumber_T)(*ptr - '0');
	    else
		un = UVARNUM_MAX;
	    ++ptr;
	    if (n++ == maxlen)
		break;
	    if ((what & STR2NR_QUOTE) && *ptr == '\''
					     && '0' <= ptr[1] && ptr[1] <= '7')
	    {
		++ptr;
		if (n++ == maxlen)
		    break;
	    }
	}
    }
    else if (pre != 0 || ((what & STR2NR_HEX) && (what & STR2NR_FORCE)))
    {
	// hex
	if (pre != 0)
	    n += 2;	    // skip over "0x"
	while (vim_isxdigit(*ptr))
	{
	    // avoid ubsan error for overflow
	    if (un <= UVARNUM_MAX / 16)
		un = 16 * un + (uvarnumber_T)hex2nr(*ptr);
	    else
		un = UVARNUM_MAX;
	    ++ptr;
	    if (n++ == maxlen)
		break;
	    if ((what & STR2NR_QUOTE) && *ptr == '\'' && vim_isxdigit(ptr[1]))
	    {
		++ptr;
		if (n++ == maxlen)
		    break;
	    }
	}
    }
    else
    {
	// decimal
	while (VIM_ISDIGIT(*ptr))
	{
	    uvarnumber_T    digit = (uvarnumber_T)(*ptr - '0');

	    // avoid ubsan error for overflow
	    if (un < UVARNUM_MAX / 10
		    || (un == UVARNUM_MAX / 10 && digit <= UVARNUM_MAX % 10))
		un = 10 * un + digit;
	    else
		un = UVARNUM_MAX;
	    ++ptr;
	    if (n++ == maxlen)
		break;
	    if ((what & STR2NR_QUOTE) && *ptr == '\'' && VIM_ISDIGIT(ptr[1]))
	    {
		++ptr;
		if (n++ == maxlen)
		    break;
	    }
	}
    }

    // Check for an alphanumeric character immediately following, that is
    // most likely a typo.
    if (strict && n - 1 != maxlen && ASCII_ISALNUM(*ptr))
	return;

    if (prep != NULL)
	*prep = pre;
    if (len != NULL)
	*len = (int)(ptr - start);
    if (nptr != NULL)
    {
	if (negative)   // account for leading '-' for decimal numbers
	{
	    // avoid ubsan error for overflow
	    if (un > VARNUM_MAX)
		*nptr = VARNUM_MIN;
	    else
		*nptr = -(varnumber_T)un;
	}
	else
	{
	    if (un > VARNUM_MAX)
		un = VARNUM_MAX;
	    *nptr = (varnumber_T)un;
	}
    }
    if (unptr != NULL)
	*unptr = un;
}