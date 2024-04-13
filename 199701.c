input_csi_dispatch_sgr_256_do(struct input_ctx *ictx, int fgbg, int c)
{
	struct grid_cell	*gc = &ictx->cell.cell;

	if (c == -1 || c > 255) {
		if (fgbg == 38)
			gc->fg = 8;
		else if (fgbg == 48)
			gc->bg = 8;
	} else {
		if (fgbg == 38)
			gc->fg = c | COLOUR_FLAG_256;
		else if (fgbg == 48)
			gc->bg = c | COLOUR_FLAG_256;
		else if (fgbg == 58)
			gc->us = c | COLOUR_FLAG_256;
	}
	return (1);
}