renderCoTable(struct table *tbl, int maxlimit)
{
    struct readbuffer obuf;
    struct html_feed_environ h_env;
    struct environment envs[MAX_ENV_LEVEL];
    struct table *t;
    int i, col, row;
    int indent, maxwidth;
    static struct table *tbl_prev = NULL;
    static int cotbl_count;
#define MAX_COTABLE 100

    if (tbl == NULL)
	return;
    if (tbl != tbl_prev) {
	tbl_prev = tbl;
	cotbl_count = 0;
    }
    if (cotbl_count >= MAX_COTABLE)
	return;	/* workaround to prevent infinite recursion */
    cotbl_count++;

    for (i = 0; i < tbl->ntable; i++) {
	t = tbl->tables[i].ptr;
	col = tbl->tables[i].col;
	row = tbl->tables[i].row;
	indent = tbl->tables[i].indent;

	init_henv(&h_env, &obuf, envs, MAX_ENV_LEVEL, tbl->tables[i].buf,
		  get_spec_cell_width(tbl, row, col), indent);
	check_row(tbl, row);
	if (h_env.limit > maxlimit)
	    h_env.limit = maxlimit;
	if (t->total_width == 0)
	    maxwidth = h_env.limit - indent;
	else if (t->total_width > 0)
	    maxwidth = t->total_width;
	else
	    maxwidth = t->total_width = -t->total_width * h_env.limit / 100;
	renderTable(t, maxwidth, &h_env);
    }
}