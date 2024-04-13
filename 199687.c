input_csi_dispatch_rm_private(struct input_ctx *ictx)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;
	struct grid_cell	*gc = &ictx->cell.cell;
	u_int			 i;

	for (i = 0; i < ictx->param_list_len; i++) {
		switch (input_get(ictx, i, 0, -1)) {
		case -1:
			break;
		case 1:		/* DECCKM */
			screen_write_mode_clear(sctx, MODE_KCURSOR);
			break;
		case 3:		/* DECCOLM */
			screen_write_cursormove(sctx, 0, 0, 1);
			screen_write_clearscreen(sctx, gc->bg);
			break;
		case 6:		/* DECOM */
			screen_write_mode_clear(sctx, MODE_ORIGIN);
			screen_write_cursormove(sctx, 0, 0, 1);
			break;
		case 7:		/* DECAWM */
			screen_write_mode_clear(sctx, MODE_WRAP);
			break;
		case 12:
			screen_write_mode_clear(sctx, MODE_BLINKING);
			break;
		case 25:	/* TCEM */
			screen_write_mode_clear(sctx, MODE_CURSOR);
			break;
		case 1000:
		case 1001:
		case 1002:
		case 1003:
			screen_write_mode_clear(sctx, ALL_MOUSE_MODES);
			break;
		case 1004:
			screen_write_mode_clear(sctx, MODE_FOCUSON);
			break;
		case 1005:
			screen_write_mode_clear(sctx, MODE_MOUSE_UTF8);
			break;
		case 1006:
			screen_write_mode_clear(sctx, MODE_MOUSE_SGR);
			break;
		case 47:
		case 1047:
			screen_write_alternateoff(sctx, gc, 0);
			break;
		case 1049:
			screen_write_alternateoff(sctx, gc, 1);
			break;
		case 2004:
			screen_write_mode_clear(sctx, MODE_BRACKETPASTE);
			break;
		default:
			log_debug("%s: unknown '%c'", __func__, ictx->ch);
			break;
		}
	}
}