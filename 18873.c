swap_tcap(void)
{
# ifdef FEAT_TERMGUICOLORS
    static int		init_done = FALSE;
    static cmode_T	curr_mode;
    struct ks_tbl_S	*ks;
    struct builtin_term *bt;
    cmode_T		mode;

    if (!init_done)
    {
	for (ks = ks_tbl; ks->code != (int)KS_NAME; ks++)
	{
	    bt = find_first_tcap(DEFAULT_TERM, ks->code);
	    if (bt != NULL)
	    {
		// Preserve the original value.
		STRNCPY(ks->buf[CMODE_INDEXED], bt->bt_string, KSSIZE);
		STRNCPY(ks->buf[CMODE_RGB], ks->vtp, KSSIZE);
		STRNCPY(ks->buf[CMODE_256COL], ks->vtp2, KSSIZE);

		bt->bt_string = ks->buf[CMODE_INDEXED];
	    }
	}
	init_done = TRUE;
	curr_mode = CMODE_INDEXED;
    }

    if (p_tgc)
	mode = CMODE_RGB;
    else if (t_colors >= 256)
	mode = CMODE_256COL;
    else
	mode = CMODE_INDEXED;

    if (mode == curr_mode)
	return;

    for (ks = ks_tbl; ks->code != (int)KS_NAME; ks++)
    {
	bt = find_first_tcap(DEFAULT_TERM, ks->code);
	if (bt != NULL)
	    bt->bt_string = ks->buf[mode];
    }

    curr_mode = mode;
# endif
}