input_csi_dispatch_winops(struct input_ctx *ictx)
{
	struct window_pane	*wp = ictx->wp;
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
		case 22:
		case 23:
			m++;
			if (input_get(ictx, m, 0, -1) == -1)
				return;
			break;
		case 18:
			input_reply(ictx, "\033[8;%u;%ut", wp->sy, wp->sx);
			break;
		default:
			log_debug("%s: unknown '%c'", __func__, ictx->ch);
			break;
		}
		m++;
	}
}