input_reset(struct window_pane *wp)
{
	struct input_ctx	*ictx = wp->ictx;

	input_reset_cell(ictx);

	if (wp->mode == NULL)
		screen_write_start(&ictx->ctx, wp, &wp->base);
	else
		screen_write_start(&ictx->ctx, NULL, &wp->base);
	screen_write_reset(&ictx->ctx);
	screen_write_stop(&ictx->ctx);
}