showoneopt(
    struct vimoption	*p,
    int			opt_flags)	/* OPT_LOCAL or OPT_GLOBAL */
{
    char_u	*varp;
    int		save_silent = silent_mode;

    silent_mode = FALSE;
    info_message = TRUE;	/* use mch_msg(), not mch_errmsg() */

    varp = get_varp_scope(p, opt_flags);

    /* for 'modified' we also need to check if 'ff' or 'fenc' changed. */
    if ((p->flags & P_BOOL) && ((int *)varp == &curbuf->b_changed
					? !curbufIsChanged() : !*(int *)varp))
	MSG_PUTS("no");
    else if ((p->flags & P_BOOL) && *(int *)varp < 0)
	MSG_PUTS("--");
    else
	MSG_PUTS("  ");
    MSG_PUTS(p->fullname);
    if (!(p->flags & P_BOOL))
    {
	msg_putchar('=');
	/* put value string in NameBuff */
	option_value2string(p, opt_flags);
	msg_outtrans(NameBuff);
    }

    silent_mode = save_silent;
    info_message = FALSE;
}