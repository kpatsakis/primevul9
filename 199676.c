input_osc_10(struct input_ctx *ictx, const char *p)
{
	struct window_pane	*wp = ictx->wp;
	struct grid_cell	 defaults;
	u_int			 r, g, b;

	if (wp == NULL)
		return;

	if (strcmp(p, "?") == 0) {
		tty_default_colours(&defaults, wp);
		input_osc_colour_reply(ictx, 10, defaults.fg);
		return;
	}

	if (!input_osc_parse_colour(p, &r, &g, &b))
		goto bad;
	wp->fg = colour_join_rgb(r, g, b);
	wp->flags |= (PANE_REDRAW|PANE_STYLECHANGED);

	return;

bad:
	log_debug("bad OSC 10: %s", p);
}