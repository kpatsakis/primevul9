input_init(struct window_pane *wp)
{
	struct input_ctx	*ictx;

	ictx = wp->ictx = xcalloc(1, sizeof *ictx);

	input_reset_cell(ictx);

	*ictx->interm_buf = '\0';
	ictx->interm_len = 0;

	*ictx->param_buf = '\0';
	ictx->param_len = 0;

	ictx->input_space = INPUT_BUF_START;
	ictx->input_buf = xmalloc(INPUT_BUF_START);

	*ictx->input_buf = '\0';
	ictx->input_len = 0;

	ictx->state = &input_state_ground;
	ictx->flags = 0;

	ictx->since_ground = evbuffer_new();
}