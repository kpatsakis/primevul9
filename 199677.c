input_csi_dispatch_sm_private(struct input_ctx *ictx)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;
	struct window_pane	*wp = ictx->wp;
	struct grid_cell	*gc = &ictx->cell.cell;
	u_int			 i;

	for (i = 0; i < ictx->param_list_len; i++) {
		switch (input_get(ictx, i, 0, -1)) {
		case -1:
			break;
		case 1:		/* DECCKM */
			screen_write_mode_set(sctx, MODE_KCURSOR);
			break;
		case 3:		/* DECCOLM */
			screen_write_cursormove(sctx, 0, 0, 1);
			screen_write_clearscreen(sctx, ictx->cell.cell.bg);
			break;
		case 6:		/* DECOM */
			screen_write_mode_set(sctx, MODE_ORIGIN);
			screen_write_cursormove(sctx, 0, 0, 1);
			break;
		case 7:		/* DECAWM */
			screen_write_mode_set(sctx, MODE_WRAP);
			break;
		case 12:
			screen_write_mode_set(sctx, MODE_BLINKING);
			break;
		case 25:	/* TCEM */
			screen_write_mode_set(sctx, MODE_CURSOR);
			break;
		case 1000:
			screen_write_mode_clear(sctx, ALL_MOUSE_MODES);
			screen_write_mode_set(sctx, MODE_MOUSE_STANDARD);
			break;
		case 1002:
			screen_write_mode_clear(sctx, ALL_MOUSE_MODES);
			screen_write_mode_set(sctx, MODE_MOUSE_BUTTON);
			break;
		case 1003:
			screen_write_mode_clear(sctx, ALL_MOUSE_MODES);
			screen_write_mode_set(sctx, MODE_MOUSE_ALL);
			break;
		case 1004:
			if (sctx->s->mode & MODE_FOCUSON)
				break;
			screen_write_mode_set(sctx, MODE_FOCUSON);
			if (wp != NULL)
				wp->flags |= PANE_FOCUSPUSH; /* force update */
			break;
		case 1005:
			screen_write_mode_set(sctx, MODE_MOUSE_UTF8);
			break;
		case 1006:
			screen_write_mode_set(sctx, MODE_MOUSE_SGR);
			break;
		case 47:
		case 1047:
			screen_write_alternateon(sctx, gc, 0);
			break;
		case 1049:
			screen_write_alternateon(sctx, gc, 1);
			break;
		case 2004:
			screen_write_mode_set(sctx, MODE_BRACKETPASTE);
			break;
		default:
			log_debug("%s: unknown '%c'", __func__, ictx->ch);
			break;
		}
	}
}