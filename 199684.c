input_print(struct input_ctx *ictx)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;
	int			 set;

	ictx->utf8started = 0; /* can't be valid UTF-8 */

	set = ictx->cell.set == 0 ? ictx->cell.g0set : ictx->cell.g1set;
	if (set == 1)
		ictx->cell.cell.attr |= GRID_ATTR_CHARSET;
	else
		ictx->cell.cell.attr &= ~GRID_ATTR_CHARSET;

	utf8_set(&ictx->cell.cell.data, ictx->ch);
	screen_write_collect_add(sctx, &ictx->cell.cell);
	ictx->last = ictx->ch;

	ictx->cell.cell.attr &= ~GRID_ATTR_CHARSET;

	return (0);
}