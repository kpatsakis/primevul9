get_user_input(
    typval_T	*argvars,
    typval_T	*rettv,
    int		inputdialog,
    int		secret)
{
    char_u	*prompt;
    char_u	*p = NULL;
    int		c;
    char_u	buf[NUMBUFLEN];
    int		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    int		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = NULL;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    if (input_busy)
	return;  // this doesn't work recursively.

    if (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	return;

    prompt = tv_get_string_chk(&argvars[0]);

#ifdef NO_CONSOLE_INPUT
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    if (no_console_input() && !is_not_a_term())
	return;
#endif

    cmd_silent = FALSE;		// Want to see the prompt.
    if (prompt != NULL)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	if (p == NULL)
	    p = prompt;
	else
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    msg_start();
	    msg_clr_eos();
	    msg_puts_attr((char *)prompt, get_echo_attr());
	    msg_didout = FALSE;
	    msg_starthere();
	    *p = c;
	}
	cmdline_row = msg_row;

	if (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    if (defstr != NULL)
		stuffReadbuffSpec(defstr);

	    if (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		int	xp_namelen;
		long	argt;

		// input() with a third argument: completion
		rettv->vval.v_string = NULL;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		if (xp_name == NULL)
		    return;

		xp_namelen = (int)STRLEN(xp_name);

		if (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    return;
	    }
	}

	if (defstr != NULL)
	{
	    int save_ex_normal_busy = ex_normal_busy;
	    int save_vgetc_busy = vgetc_busy;
	    int save_input_busy = input_busy;

	    input_busy |= vgetc_busy;
	    ex_normal_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_normal_busy = save_ex_normal_busy;
	    vgetc_busy = save_vgetc_busy;
	    input_busy = save_input_busy;
	}
	if (inputdialog && rettv->vval.v_string == NULL
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	msg_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}