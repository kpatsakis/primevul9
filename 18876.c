got_code_from_term(char_u *code, int len)
{
#define XT_LEN 100
    char_u	name[3];
    char_u	str[XT_LEN];
    int		i;
    int		j = 0;
    int		c;

    // A '1' means the code is supported, a '0' means it isn't.
    // When half the length is > XT_LEN we can't use it.
    // Our names are currently all 2 characters.
    if (code[0] == '1' && code[7] == '=' && len / 2 < XT_LEN)
    {
	// Get the name from the response and find it in the table.
	name[0] = hexhex2nr(code + 3);
	name[1] = hexhex2nr(code + 5);
	name[2] = NUL;
	for (i = 0; key_names[i] != NULL; ++i)
	{
	    if (STRCMP(key_names[i], name) == 0)
	    {
		xt_index_in = i;
		break;
	    }
	}

	LOG_TR(("Received XT %d: %s", xt_index_in, (char *)name));

	if (key_names[i] != NULL)
	{
	    for (i = 8; (c = hexhex2nr(code + i)) >= 0; i += 2)
		str[j++] = c;
	    str[j] = NUL;
	    if (name[0] == 'C' && name[1] == 'o')
	    {
		// Color count is not a key code.
		may_adjust_color_count(atoi((char *)str));
	    }
	    else
	    {
		// First delete any existing entry with the same code.
		i = find_term_bykeys(str);
		if (i >= 0)
		    del_termcode_idx(i);
		add_termcode(name, str, ATC_FROM_TERM);
	    }
	}
    }

    // May request more codes now that we received one.
    ++xt_index_in;
    req_more_codes_from_term();
}