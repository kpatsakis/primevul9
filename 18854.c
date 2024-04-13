add_termcode(char_u *name, char_u *string, int flags)
{
    struct termcode *new_tc;
    int		    i, j;
    char_u	    *s;
    int		    len;

    if (string == NULL || *string == NUL)
    {
	del_termcode(name);
	return;
    }

#if defined(MSWIN) && !defined(FEAT_GUI)
    s = vim_strnsave(string, STRLEN(string) + 1);
#else
# ifdef VIMDLL
    if (!gui.in_use)
	s = vim_strnsave(string, STRLEN(string) + 1);
    else
# endif
	s = vim_strsave(string);
#endif
    if (s == NULL)
	return;

    // Change leading <Esc>[ to CSI, change <Esc>O to <M-O>.
    if (flags != 0 && flags != ATC_FROM_TERM && term_7to8bit(string) != 0)
    {
	STRMOVE(s, s + 1);
	s[0] = term_7to8bit(string);
    }

#if defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
# ifdef VIMDLL
    if (!gui.in_use)
# endif
    {
	if (s[0] == K_NUL)
	{
	    STRMOVE(s + 1, s);
	    s[1] = 3;
	}
    }
#endif

    len = (int)STRLEN(s);

    need_gather = TRUE;		// need to fill termleader[]

    /*
     * need to make space for more entries
     */
    if (tc_len == tc_max_len)
    {
	tc_max_len += 20;
	new_tc = ALLOC_MULT(struct termcode, tc_max_len);
	if (new_tc == NULL)
	{
	    tc_max_len -= 20;
	    vim_free(s);
	    return;
	}
	for (i = 0; i < tc_len; ++i)
	    new_tc[i] = termcodes[i];
	vim_free(termcodes);
	termcodes = new_tc;
    }

    /*
     * Look for existing entry with the same name, it is replaced.
     * Look for an existing entry that is alphabetical higher, the new entry
     * is inserted in front of it.
     */
    for (i = 0; i < tc_len; ++i)
    {
	if (termcodes[i].name[0] < name[0])
	    continue;
	if (termcodes[i].name[0] == name[0])
	{
	    if (termcodes[i].name[1] < name[1])
		continue;
	    /*
	     * Exact match: May replace old code.
	     */
	    if (termcodes[i].name[1] == name[1])
	    {
		if (flags == ATC_FROM_TERM && (j = termcode_star(
				    termcodes[i].code, termcodes[i].len)) > 0)
		{
		    // Don't replace ESC[123;*X or ESC O*X with another when
		    // invoked from got_code_from_term().
		    if (len == termcodes[i].len - j
			    && STRNCMP(s, termcodes[i].code, len - 1) == 0
			    && s[len - 1]
				   == termcodes[i].code[termcodes[i].len - 1])
		    {
			// They are equal but for the ";*": don't add it.
			vim_free(s);
			return;
		    }
		}
		else
		{
		    // Replace old code.
		    vim_free(termcodes[i].code);
		    --tc_len;
		    break;
		}
	    }
	}
	/*
	 * Found alphabetical larger entry, move rest to insert new entry
	 */
	for (j = tc_len; j > i; --j)
	    termcodes[j] = termcodes[j - 1];
	break;
    }

    termcodes[i].name[0] = name[0];
    termcodes[i].name[1] = name[1];
    termcodes[i].code = s;
    termcodes[i].len = len;

    // For xterm we recognize special codes like "ESC[42;*X" and "ESC O*X" that
    // accept modifiers.
    termcodes[i].modlen = 0;
    j = termcode_star(s, len);
    if (j > 0)
    {
	termcodes[i].modlen = len - 1 - j;
	// For "CSI[@;X" the "@" is not included in "modlen".
	if (termcodes[i].code[termcodes[i].modlen - 1] == '@')
	    --termcodes[i].modlen;
    }
    ++tc_len;
}