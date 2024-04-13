input_csi_dispatch_sm_private(struct input_ctx *ictx)
{
	struct window_pane	*wp = ictx->wp;
	u_int			 i;

	for (i = 0; i < ictx->param_list_len; i++) {
		switch (input_get(ictx, i, 0, -1)) {
		case 1:		/* DECCKM */
			screen_write_mode_set(&ictx->ctx, MODE_KCURSOR);
			break;
		case 3:		/* DECCOLM */
			screen_write_cursormove(&ictx->ctx, 0, 0);
			screen_write_clearscreen(&ictx->ctx);
			break;
		case 7:		/* DECAWM */
			screen_write_mode_set(&ictx->ctx, MODE_WRAP);
			break;
		case 12:
			screen_write_mode_set(&ictx->ctx, MODE_BLINKING);
			break;
		case 25:	/* TCEM */
			screen_write_mode_set(&ictx->ctx, MODE_CURSOR);
			break;
		case 1000:
			screen_write_mode_clear(&ictx->ctx, ALL_MOUSE_MODES);
			screen_write_mode_set(&ictx->ctx, MODE_MOUSE_STANDARD);
			break;
		case 1002:
			screen_write_mode_clear(&ictx->ctx, ALL_MOUSE_MODES);
			screen_write_mode_set(&ictx->ctx, MODE_MOUSE_BUTTON);
			break;
		case 1004:
			if (ictx->ctx.s->mode & MODE_FOCUSON)
				break;
			screen_write_mode_set(&ictx->ctx, MODE_FOCUSON);
			wp->flags |= PANE_FOCUSPUSH; /* force update */
			break;
		case 1005:
			screen_write_mode_set(&ictx->ctx, MODE_MOUSE_UTF8);
			break;
		case 1006:
			screen_write_mode_set(&ictx->ctx, MODE_MOUSE_SGR);
			break;
		case 47:
		case 1047:
			window_pane_alternate_on(wp, &ictx->cell.cell, 0);
			break;
		case 1049:
			window_pane_alternate_on(wp, &ictx->cell.cell, 1);
			break;
		case 2004:
			screen_write_mode_set(&ictx->ctx, MODE_BRACKETPASTE);
			break;
		default:
			log_debug("%s: unknown '%c'", __func__, ictx->ch);
			break;
		}
	}
}