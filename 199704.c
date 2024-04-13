input_osc_4(struct input_ctx *ictx, const char *p)
{
	struct window_pane	*wp = ictx->wp;
	char			*copy, *s, *next = NULL;
	long			 idx;
	u_int			 r, g, b;

	if (wp == NULL)
		return;

	copy = s = xstrdup(p);
	while (s != NULL && *s != '\0') {
		idx = strtol(s, &next, 10);
		if (*next++ != ';')
			goto bad;
		if (idx < 0 || idx >= 0x100)
			goto bad;

		s = strsep(&next, ";");
		if (!input_osc_parse_colour(s, &r, &g, &b)) {
			s = next;
			continue;
		}

		window_pane_set_palette(wp, idx, colour_join_rgb(r, g, b));
		s = next;
	}

	free(copy);
	return;

bad:
	log_debug("bad OSC 4: %s", p);
	free(copy);
}