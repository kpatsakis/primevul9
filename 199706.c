input_csi_dispatch_rm(struct input_ctx *ictx)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;
	u_int			 i;

	for (i = 0; i < ictx->param_list_len; i++) {
		switch (input_get(ictx, i, 0, -1)) {
		case -1:
			break;
		case 4:		/* IRM */
			screen_write_mode_clear(sctx, MODE_INSERT);
			break;
		case 34:
			screen_write_mode_set(sctx, MODE_BLINKING);
			break;
		default:
			log_debug("%s: unknown '%c'", __func__, ictx->ch);
			break;
		}
	}
}