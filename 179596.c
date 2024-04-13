input_exit_rename(struct input_ctx *ictx)
{
	if (ictx->flags & INPUT_DISCARD)
		return;
	if (!options_get_number(&ictx->wp->window->options, "allow-rename"))
		return;
	log_debug("%s: \"%s\"", __func__, ictx->input_buf);

	window_set_name(ictx->wp->window, ictx->input_buf);
	options_set_number(&ictx->wp->window->options, "automatic-rename", 0);

	server_status_window(ictx->wp->window);
}