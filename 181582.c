f_test_override(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u *name = (char_u *)"";
    int     val;
    static int save_starting = -1;

    if (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL)
	return;

    name = tv_get_string(&argvars[0]);
    val = (int)tv_get_number(&argvars[1]);

    if (STRCMP(name, (char_u *)"redraw") == 0)
	disable_redraw_for_testing = val;
    else if (STRCMP(name, (char_u *)"redraw_flag") == 0)
	ignore_redraw_flag_for_testing = val;
    else if (STRCMP(name, (char_u *)"char_avail") == 0)
	disable_char_avail_for_testing = val;
    else if (STRCMP(name, (char_u *)"starting") == 0)
    {
	if (val)
	{
	    if (save_starting < 0)
		save_starting = starting;
	    starting = 0;
	}
	else
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    else if (STRCMP(name, (char_u *)"nfa_fail") == 0)
	nfa_fail_for_testing = val;
    else if (STRCMP(name, (char_u *)"no_query_mouse") == 0)
	no_query_mouse_for_testing = val;
    else if (STRCMP(name, (char_u *)"no_wait_return") == 0)
	no_wait_return = val;
    else if (STRCMP(name, (char_u *)"ui_delay") == 0)
	ui_delay_for_testing = val;
    else if (STRCMP(name, (char_u *)"term_props") == 0)
	reset_term_props_on_termresponse = val;
    else if (STRCMP(name, (char_u *)"vterm_title") == 0)
	disable_vterm_title_for_testing = val;
    else if (STRCMP(name, (char_u *)"uptime") == 0)
	override_sysinfo_uptime = val;
    else if (STRCMP(name, (char_u *)"alloc_lines") == 0)
	ml_get_alloc_lines = val;
    else if (STRCMP(name, (char_u *)"autoload") == 0)
	override_autoload = val;
    else if (STRCMP(name, (char_u *)"ALL") == 0)
    {
	disable_char_avail_for_testing = FALSE;
	disable_redraw_for_testing = FALSE;
	ignore_redraw_flag_for_testing = FALSE;
	nfa_fail_for_testing = FALSE;
	no_query_mouse_for_testing = FALSE;
	ui_delay_for_testing = 0;
	reset_term_props_on_termresponse = FALSE;
	override_sysinfo_uptime = -1;
	// ml_get_alloc_lines is not reset by "ALL"
	if (save_starting >= 0)
	{
	    starting = save_starting;
	    save_starting = -1;
	}
    }
    else
	semsg(_(e_invalid_argument_str), name);
}