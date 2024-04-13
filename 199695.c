input_csi_dispatch_winops(struct input_ctx *ictx)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;
	struct screen		*s = sctx->s;
	struct window_pane	*wp = ictx->wp;
	u_int			 x = screen_size_x(s), y = screen_size_y(s);
	int			 n, m;

	m = 0;
	while ((n = input_get(ictx, m, 0, -1)) != -1) {
		switch (n) {
		case 1:
		case 2:
		case 5:
		case 6:
		case 7:
		case 11:
		case 13:
		case 14:
		case 19:
		case 20:
		case 21:
		case 24:
			break;
		case 3:
		case 4:
		case 8:
			m++;
			if (input_get(ictx, m, 0, -1) == -1)
				return;
			/* FALLTHROUGH */
		case 9:
		case 10:
			m++;
			if (input_get(ictx, m, 0, -1) == -1)
				return;
			break;
		case 22:
			m++;
			switch (input_get(ictx, m, 0, -1)) {
			case -1:
				return;
			case 0:
			case 2:
				screen_push_title(sctx->s);
				break;
			}
			break;
		case 23:
			m++;
			switch (input_get(ictx, m, 0, -1)) {
			case -1:
				return;
			case 0:
			case 2:
				screen_pop_title(sctx->s);
				if (wp != NULL) {
					notify_pane("pane-title-changed", wp);
					server_redraw_window_borders(wp->window);
					server_status_window(wp->window);
				}
				break;
			}
			break;
		case 18:
			input_reply(ictx, "\033[8;%u;%ut", x, y);
			break;
		default:
			log_debug("%s: unknown '%c'", __func__, ictx->ch);
			break;
		}
		m++;
	}
}