showoptions(
    int		all,
    int		opt_flags)	/* OPT_LOCAL and/or OPT_GLOBAL */
{
    struct vimoption	*p;
    int			col;
    int			isterm;
    char_u		*varp;
    struct vimoption	**items;
    int			item_count;
    int			run;
    int			row, rows;
    int			cols;
    int			i;
    int			len;

#define INC 20
#define GAP 3

    items = (struct vimoption **)alloc((unsigned)(sizeof(struct vimoption *) *
								PARAM_COUNT));
    if (items == NULL)
	return;

    /* Highlight title */
    if (all == 2)
	MSG_PUTS_TITLE(_("\n--- Terminal codes ---"));
    else if (opt_flags & OPT_GLOBAL)
	MSG_PUTS_TITLE(_("\n--- Global option values ---"));
    else if (opt_flags & OPT_LOCAL)
	MSG_PUTS_TITLE(_("\n--- Local option values ---"));
    else
	MSG_PUTS_TITLE(_("\n--- Options ---"));

    /*
     * do the loop two times:
     * 1. display the short items
     * 2. display the long items (only strings and numbers)
     */
    for (run = 1; run <= 2 && !got_int; ++run)
    {
	/*
	 * collect the items in items[]
	 */
	item_count = 0;
	for (p = &options[0]; p->fullname != NULL; p++)
	{
	    varp = NULL;
	    isterm = istermoption(p);
	    if (opt_flags != 0)
	    {
		if (p->indir != PV_NONE && !isterm)
		    varp = get_varp_scope(p, opt_flags);
	    }
	    else
		varp = get_varp(p);
	    if (varp != NULL
		    && ((all == 2 && isterm)
			|| (all == 1 && !isterm)
			|| (all == 0 && !optval_default(p, varp))))
	    {
		if (p->flags & P_BOOL)
		    len = 1;		/* a toggle option fits always */
		else
		{
		    option_value2string(p, opt_flags);
		    len = (int)STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		if ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    items[item_count++] = p;
	    }
	}

	/*
	 * display the items
	 */
	if (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    if (cols == 0)
		cols = 1;
	    rows = (item_count + cols - 1) / cols;
	}
	else	/* run == 2 */
	    rows = item_count;
	for (row = 0; row < rows && !got_int; ++row)
	{
	    msg_putchar('\n');			/* go to next line */
	    if (got_int)			/* 'q' typed in more */
		break;
	    col = 0;
	    for (i = row; i < item_count; i += rows)
	    {
		msg_col = col;			/* make columns */
		showoneopt(items[i], opt_flags);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(items);
}