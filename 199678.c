input_exit_osc(struct input_ctx *ictx)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;
	struct window_pane	*wp = ictx->wp;
	u_char			*p = ictx->input_buf;
	u_int			 option;

	if (ictx->flags & INPUT_DISCARD)
		return;
	if (ictx->input_len < 1 || *p < '0' || *p > '9')
		return;

	log_debug("%s: \"%s\" (end %s)", __func__, p,
	    ictx->input_end == INPUT_END_ST ? "ST" : "BEL");

	option = 0;
	while (*p >= '0' && *p <= '9')
		option = option * 10 + *p++ - '0';
	if (*p == ';')
		p++;

	switch (option) {
	case 0:
	case 2:
		if (screen_set_title(sctx->s, p) && wp != NULL) {
			notify_pane("pane-title-changed", wp);
			server_redraw_window_borders(wp->window);
			server_status_window(wp->window);
		}
		break;
	case 4:
		input_osc_4(ictx, p);
		break;
	case 7:
		if (utf8_isvalid(p)) {
			screen_set_path(sctx->s, p);
			if (wp != NULL) {
				server_redraw_window_borders(wp->window);
				server_status_window(wp->window);
			}
		}
		break;
	case 10:
		input_osc_10(ictx, p);
		break;
	case 11:
		input_osc_11(ictx, p);
		break;
	case 12:
		if (utf8_isvalid(p) && *p != '?') /* ? is colour request */
			screen_set_cursor_colour(sctx->s, p);
		break;
	case 52:
		input_osc_52(ictx, p);
		break;
	case 104:
		input_osc_104(ictx, p);
		break;
	case 112:
		if (*p == '\0') /* no arguments allowed */
			screen_set_cursor_colour(sctx->s, "");
		break;
	default:
		log_debug("%s: unknown '%u'", __func__, option);
		break;
	}
}