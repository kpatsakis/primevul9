input_utf8_close(struct input_ctx *ictx)
{
	log_debug("%s", __func__);

	utf8_append(&ictx->utf8data, ictx->ch);

	grid_cell_set(&ictx->cell.cell, &ictx->utf8data);
	screen_write_cell(&ictx->ctx, &ictx->cell.cell);

	return (0);
}