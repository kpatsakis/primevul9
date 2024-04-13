input_parse_screen(struct input_ctx *ictx, struct screen *s,
    screen_write_init_ctx_cb cb, void *arg, u_char *buf, size_t len)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;

	if (len == 0)
		return;

	screen_write_start_callback(sctx, s, cb, arg);
	input_parse(ictx, buf, len);
	screen_write_stop(sctx);
}