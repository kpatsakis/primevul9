input_exit_osc(struct input_ctx *ictx)
{
	u_char	*p = ictx->input_buf;
	u_int	 option;

	if (ictx->flags & INPUT_DISCARD)
		return;
	if (ictx->input_len < 1 || *p < '0' || *p > '9')
		return;

	log_debug("%s: \"%s\"", __func__, p);

	option = 0;
	while (*p >= '0' && *p <= '9')
		option = option * 10 + *p++ - '0';
	if (*p == ';')
		p++;

	switch (option) {
	case 0:
	case 2:
		screen_set_title(ictx->ctx.s, p);
		server_status_window(ictx->wp->window);
		break;
	case 12:
		if (*p != '?') /* ? is colour request */
			screen_set_cursor_colour(ictx->ctx.s, p);
		break;
	case 112:
		if (*p == '\0') /* no arguments allowed */
			screen_set_cursor_colour(ictx->ctx.s, "");
		break;
	default:
		log_debug("%s: unknown '%u'", __func__, option);
		break;
	}
}