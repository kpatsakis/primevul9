handle_csi(
	char_u	*tp,
	int	len,
	char_u	*argp,
	int	offset,
	char_u  *buf,
	int	bufsize,
	int	*buflen,
	char_u	*key_name,
	int	*slen)
{
    int		first = -1;  // optional char right after {lead}
    int		trail;	     // char that ends CSI sequence
    int		arg[3] = {-1, -1, -1};	// argument numbers
    int		argc;			// number of arguments
    char_u	*ap = argp;
    int		csi_len;

    // Check for non-digit after CSI.
    if (!VIM_ISDIGIT(*ap))
	first = *ap++;

    // Find up to three argument numbers.
    for (argc = 0; argc < 3; )
    {
	if (ap >= tp + len)
	    return -1;
	if (*ap == ';')
	    arg[argc++] = -1;  // omitted number
	else if (VIM_ISDIGIT(*ap))
	{
	    arg[argc] = 0;
	    for (;;)
	    {
		if (ap >= tp + len)
		    return -1;
		if (!VIM_ISDIGIT(*ap))
		    break;
		arg[argc] = arg[argc] * 10 + (*ap - '0');
		++ap;
	    }
	    ++argc;
	}
	if (*ap == ';')
	    ++ap;
	else
	    break;
    }

    // mrxvt has been reported to have "+" in the version. Assume
    // the escape sequence ends with a letter or one of "{|}~".
    while (ap < tp + len
	    && !(*ap >= '{' && *ap <= '~')
	    && !ASCII_ISALPHA(*ap))
	++ap;
    if (ap >= tp + len)
	return -1;
    trail = *ap;
    csi_len = (int)(ap - tp) + 1;

    // Cursor position report: Eat it when there are 2 arguments
    // and it ends in 'R'. Also when u7_status is not "sent", it
    // may be from a previous Vim that just exited.  But not for
    // <S-F3>, it sends something similar, check for row and column
    // to make sense.
    if (first == -1 && argc == 2 && trail == 'R')
    {
	handle_u7_response(arg, tp, csi_len);

	key_name[0] = (int)KS_EXTRA;
	key_name[1] = (int)KE_IGNORE;
	*slen = csi_len;
    }

    // Version string: Eat it when there is at least one digit and
    // it ends in 'c'
    else if (*T_CRV != NUL && ap > argp + 1 && trail == 'c')
    {
	handle_version_response(first, arg, argc, tp);

	*slen = csi_len;
# ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMRESPONSE, tp, *slen);
# endif
	apply_autocmds(EVENT_TERMRESPONSE,
					NULL, NULL, FALSE, curbuf);
	key_name[0] = (int)KS_EXTRA;
	key_name[1] = (int)KE_IGNORE;
    }

    // Check blinking cursor from xterm:
    // {lead}?12;1$y       set
    // {lead}?12;2$y       not set
    //
    // {lead} can be <Esc>[ or CSI
    else if (rbm_status.tr_progress == STATUS_SENT
	    && first == '?'
	    && ap == argp + 6
	    && arg[0] == 12
	    && ap[-1] == '$'
	    && trail == 'y')
    {
	initial_cursor_blink = (arg[1] == '1');
	rbm_status.tr_progress = STATUS_GOT;
	LOG_TR(("Received cursor blinking mode response: %s", tp));
	key_name[0] = (int)KS_EXTRA;
	key_name[1] = (int)KE_IGNORE;
	*slen = csi_len;
# ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMBLINKRESP, tp, *slen);
# endif
    }

    // Check for a window position response from the terminal:
    //       {lead}3;{x};{y}t
    else if (did_request_winpos && argc == 3 && arg[0] == 3
						   && trail == 't')
    {
	winpos_x = arg[1];
	winpos_y = arg[2];
	// got finished code: consume it
	key_name[0] = (int)KS_EXTRA;
	key_name[1] = (int)KE_IGNORE;
	*slen = csi_len;

	if (--did_request_winpos <= 0)
	    winpos_status.tr_progress = STATUS_GOT;
    }

    // Key with modifier:
    //	{lead}27;{modifier};{key}~
    //	{lead}{key};{modifier}u
    else if ((arg[0] == 27 && argc == 3 && trail == '~')
	    || (argc == 2 && trail == 'u'))
    {
	return len + handle_key_with_modifier(arg, trail,
			    csi_len, offset, buf, bufsize, buflen);
    }

    // else: Unknown CSI sequence.  We could drop it, but then the
    // user can't create a map for it.
    return 0;
}