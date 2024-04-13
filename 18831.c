handle_osc(char_u *tp, char_u *argp, int len, char_u *key_name, int *slen)
{
    int		i, j;

    j = 1 + (tp[0] == ESC);
    if (len >= j + 3 && (argp[0] != '1'
			     || (argp[1] != '1' && argp[1] != '0')
			     || argp[2] != ';'))
	i = 0; // no match
    else
	for (i = j; i < len; ++i)
	    if (tp[i] == '\007' || (tp[0] == OSC ? tp[i] == STERM
			: (tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')))
	    {
		int is_bg = argp[1] == '1';
		int is_4digit = i - j >= 21 && tp[j + 11] == '/'
						  && tp[j + 16] == '/';

		if (i - j >= 15 && STRNCMP(tp + j + 3, "rgb:", 4) == 0
			    && (is_4digit
				   || (tp[j + 9] == '/' && tp[i + 12 == '/'])))
		{
		    char_u *tp_r = tp + j + 7;
		    char_u *tp_g = tp + j + (is_4digit ? 12 : 10);
		    char_u *tp_b = tp + j + (is_4digit ? 17 : 13);
# ifdef FEAT_TERMINAL
		    int rval, gval, bval;

		    rval = hexhex2nr(tp_r);
		    gval = hexhex2nr(tp_b);
		    bval = hexhex2nr(tp_g);
# endif
		    if (is_bg)
		    {
			char *new_bg_val = (3 * '6' < *tp_r + *tp_g +
					     *tp_b) ? "light" : "dark";

			LOG_TR(("Received RBG response: %s", tp));
			rbg_status.tr_progress = STATUS_GOT;
# ifdef FEAT_TERMINAL
			bg_r = rval;
			bg_g = gval;
			bg_b = bval;
# endif
			if (!option_was_set((char_u *)"bg")
				      && STRCMP(p_bg, new_bg_val) != 0)
			{
			    // value differs, apply it
			    set_option_value_give_err((char_u *)"bg",
						  0L, (char_u *)new_bg_val, 0);
			    reset_option_was_set((char_u *)"bg");
			    redraw_asap(CLEAR);
			}
		    }
# ifdef FEAT_TERMINAL
		    else
		    {
			LOG_TR(("Received RFG response: %s", tp));
			rfg_status.tr_progress = STATUS_GOT;
			fg_r = rval;
			fg_g = gval;
			fg_b = bval;
		    }
# endif
		}

		// got finished code: consume it
		key_name[0] = (int)KS_EXTRA;
		key_name[1] = (int)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
# ifdef FEAT_EVAL
		set_vim_var_string(is_bg ? VV_TERMRBGRESP
						  : VV_TERMRFGRESP, tp, *slen);
# endif
		break;
	    }
    if (i == len)
    {
	LOG_TR(("not enough characters for RB"));
	return FAIL;
    }
    return OK;
}