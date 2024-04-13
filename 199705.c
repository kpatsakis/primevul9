input_osc_104(struct input_ctx *ictx, const char *p)
{
	struct window_pane	*wp = ictx->wp;
	char			*copy, *s;
	long			 idx;

	if (wp == NULL)
		return;

	if (*p == '\0') {
		window_pane_reset_palette(wp);
		return;
	}

	copy = s = xstrdup(p);
	while (*s != '\0') {
		idx = strtol(s, &s, 10);
		if (*s != '\0' && *s != ';')
			goto bad;
		if (idx < 0 || idx >= 0x100)
			goto bad;

		window_pane_unset_palette(wp, idx);
		if (*s == ';')
			s++;
	}
	free(copy);
	return;

bad:
	log_debug("bad OSC 104: %s", p);
	free(copy);
}