input_exit_apc(struct input_ctx *ictx)
{
	if (ictx->flags & INPUT_DISCARD)
		return;
	log_debug("%s: \"%s\"", __func__, ictx->input_buf);

	screen_set_title(ictx->ctx.s, ictx->input_buf);
	server_status_window(ictx->wp->window);
}