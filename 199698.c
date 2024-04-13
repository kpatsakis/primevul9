input_restore_state(struct input_ctx *ictx)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;

	memcpy(&ictx->cell, &ictx->old_cell, sizeof ictx->cell);
	if (ictx->old_mode & MODE_ORIGIN)
		screen_write_mode_set(sctx, MODE_ORIGIN);
	else
		screen_write_mode_clear(sctx, MODE_ORIGIN);
	screen_write_cursormove(sctx, ictx->old_cx, ictx->old_cy, 0);
}