set_cmdline_str(char_u *str, int pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    int		    len;

    if (p == NULL)
	return 1;

    len = (int)STRLEN(str);
    realloc_cmdbuff(len + 1);
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    return 0;
}