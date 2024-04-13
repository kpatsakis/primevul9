check_clipboard_option(void)
{
    int		new_unnamed = 0;
    int		new_autoselect_star = FALSE;
    int		new_autoselect_plus = FALSE;
    int		new_autoselectml = FALSE;
    int		new_html = FALSE;
    regprog_T	*new_exclude_prog = NULL;
    char_u	*errmsg = NULL;
    char_u	*p;

    for (p = p_cb; *p != NUL; )
    {
	if (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	else if (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
	else if (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_star = TRUE;
	    p += 10;
	}
	else if (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	else if (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	else if (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	else if (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == NULL)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    if (new_exclude_prog == NULL)
		errmsg = e_invarg;
	    break;
	}
	else
	{
	    errmsg = e_invarg;
	    break;
	}
	if (*p == ',')
	    ++p;
    }
    if (errmsg == NULL)
    {
	clip_unnamed = new_unnamed;
	clip_autoselect_star = new_autoselect_star;
	clip_autoselect_plus = new_autoselect_plus;
	clip_autoselectml = new_autoselectml;
	clip_html = new_html;
	vim_regfree(clip_exclude_prog);
	clip_exclude_prog = new_exclude_prog;
#ifdef FEAT_GUI_GTK
	if (gui.in_use)
	{
	    gui_gtk_set_selection_targets();
	    gui_gtk_set_dnd_targets();
	}
#endif
    }
    else
	vim_regfree(new_exclude_prog);

    return errmsg;
}