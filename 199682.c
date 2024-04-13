input_osc_11(struct input_ctx *ictx, const char *p)
{
	struct window_pane	*wp = ictx->wp;
	struct grid_cell	 defaults;
	u_int			 r, g, b;

	if (wp == NULL)
		return;

	if (strcmp(p, "?") == 0) {
		tty_default_colours(&defaults, wp);
		input_osc_colour_reply(ictx, 11, defaults.bg);
		return;
	}

	if (!input_osc_parse_colour(p, &r, &g, &b))
	    goto bad;
	wp->bg = colour_join_rgb(r, g, b);
	wp->flags |= (PANE_REDRAW|PANE_STYLECHANGED);

	return;

bad:
	log_debug("bad OSC 11: %s", p);
}