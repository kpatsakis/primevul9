input_free(struct window_pane *wp)
{
	struct input_ctx	*ictx = wp->ictx;

	free(ictx->input_buf);
	evbuffer_free(ictx->since_ground);

	free (ictx);
	wp->ictx = NULL;
}