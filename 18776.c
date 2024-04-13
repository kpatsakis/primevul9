parse_builtin_tcap(char_u *term)
{
    struct builtin_term	    *p;
    char_u		    name[2];
    int			    term_8bit;

    p = find_builtin_term(term);
    term_8bit = term_is_8bit(term);

    // Do not parse if builtin term not found
    if (p->bt_string == NULL)
	return;

    for (++p; p->bt_entry != (int)KS_NAME && p->bt_entry != BT_EXTRA_KEYS; ++p)
    {
	if ((int)p->bt_entry >= 0)	// KS_xx entry
	{
	    // Only set the value if it wasn't set yet.
	    if (term_strings[p->bt_entry] == NULL
				 || term_strings[p->bt_entry] == empty_option)
	    {
#ifdef FEAT_EVAL
		int opt_idx = -1;
#endif
		// 8bit terminal: use CSI instead of <Esc>[
		if (term_8bit && term_7to8bit((char_u *)p->bt_string) != 0)
		{
		    char_u  *s, *t;

		    s = vim_strsave((char_u *)p->bt_string);
		    if (s != NULL)
		    {
			for (t = s; *t; ++t)
			    if (term_7to8bit(t))
			    {
				*t = term_7to8bit(t);
				STRMOVE(t + 1, t + 2);
			    }
			term_strings[p->bt_entry] = s;
#ifdef FEAT_EVAL
			opt_idx =
#endif
				  set_term_option_alloced(
						   &term_strings[p->bt_entry]);
		    }
		}
		else
		{
		    term_strings[p->bt_entry] = (char_u *)p->bt_string;
#ifdef FEAT_EVAL
		    opt_idx = get_term_opt_idx(&term_strings[p->bt_entry]);
#endif
		}
#ifdef FEAT_EVAL
		set_term_option_sctx_idx(NULL, opt_idx);
#endif
	    }
	}
	else
	{
	    name[0] = KEY2TERMCAP0((int)p->bt_entry);
	    name[1] = KEY2TERMCAP1((int)p->bt_entry);
	    if (find_termcode(name) == NULL)
		add_termcode(name, (char_u *)p->bt_string, term_8bit);
	}
    }
}