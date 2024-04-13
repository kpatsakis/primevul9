input_print(struct input_ctx *ictx)
{
	int	set;

	set = ictx->cell.set == 0 ? ictx->cell.g0set : ictx->cell.g1set;
	if (set == 1)
		ictx->cell.cell.attr |= GRID_ATTR_CHARSET;
	else
		ictx->cell.cell.attr &= ~GRID_ATTR_CHARSET;

	grid_cell_one(&ictx->cell.cell, ictx->ch);
	screen_write_cell(&ictx->ctx, &ictx->cell.cell);

	ictx->cell.cell.attr &= ~GRID_ATTR_CHARSET;

	return (0);
}