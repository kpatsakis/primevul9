handle_dcs(char_u *tp, char_u *argp, int len, char_u *key_name, int *slen)
{
    int i, j;

    j = 1 + (tp[0] == ESC);
    if (len < j + 3)
	i = len; // need more chars
    else if ((argp[1] != '+' && argp[1] != '$') || argp[2] != 'r')
	i = 0; // no match
    else if (argp[1] == '+')
	// key code response
	for (i = j; i < len; ++i)
	{
	    if ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
		    || tp[i] == STERM)
	    {
		if (i - j >= 3)
		    got_code_from_term(tp + j, i);
		key_name[0] = (int)KS_EXTRA;
		key_name[1] = (int)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
		break;
	    }
	}
    else
    {
	// Probably the cursor shape response.  Make sure that "i"
	// is equal to "len" when there are not sufficient
	// characters.
	for (i = j + 3; i < len; ++i)
	{
	    if (i - j == 3 && !isdigit(tp[i]))
		break;
	    if (i - j == 4 && tp[i] != ' ')
		break;
	    if (i - j == 5 && tp[i] != 'q')
		break;
	    if (i - j == 6 && tp[i] != ESC && tp[i] != STERM)
		break;
	    if ((i - j == 6 && tp[i] == STERM)
	     || (i - j == 7 && tp[i] == '\\'))
	    {
		int number = argp[3] - '0';

		// 0, 1 = block blink, 2 = block
		// 3 = underline blink, 4 = underline
		// 5 = vertical bar blink, 6 = vertical bar
		number = number == 0 ? 1 : number;
		initial_cursor_shape = (number + 1) / 2;
		// The blink flag is actually inverted, compared to
		// the value set with T_SH.
		initial_cursor_shape_blink =
				       (number & 1) ? FALSE : TRUE;
		rcs_status.tr_progress = STATUS_GOT;
		LOG_TR(("Received cursor shape response: %s", tp));

		key_name[0] = (int)KS_EXTRA;
		key_name[1] = (int)KE_IGNORE;
		*slen = i + 1;
# ifdef FEAT_EVAL
		set_vim_var_string(VV_TERMSTYLERESP, tp, *slen);
# endif
		break;
	    }
	}
    }

    if (i == len)
    {
	// These codes arrive many together, each code can be
	// truncated at any point.
	LOG_TR(("not enough characters for XT"));
	return FAIL;
    }
    return OK;
}