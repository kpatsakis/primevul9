input_csi_dispatch_sgr_rgb_do(struct input_ctx *ictx, int fgbg, int r, int g,
    int b)
{
	struct grid_cell	*gc = &ictx->cell.cell;

	if (r == -1 || r > 255)
		return (0);
	if (g == -1 || g > 255)
		return (0);
	if (b == -1 || b > 255)
		return (0);

	if (fgbg == 38)
		gc->fg = colour_join_rgb(r, g, b);
	else if (fgbg == 48)
		gc->bg = colour_join_rgb(r, g, b);
	else if (fgbg == 58)
		gc->us = colour_join_rgb(r, g, b);
	return (1);
}