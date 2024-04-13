input_csi_dispatch_sm(struct input_ctx *ictx)
{
	u_int	i;

	for (i = 0; i < ictx->param_list_len; i++) {
		switch (input_get(ictx, i, 0, -1)) {
		case 4:		/* IRM */
			screen_write_mode_set(&ictx->ctx, MODE_INSERT);
			break;
		case 34:
			screen_write_mode_clear(&ictx->ctx, MODE_BLINKING);
			break;
		default:
			log_debug("%s: unknown '%c'", __func__, ictx->ch);
			break;
		}
	}
}