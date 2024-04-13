input_csi_dispatch_sgr_rgb(struct input_ctx *ictx, int fgbg, u_int *i)
{
	struct grid_cell	*gc = &ictx->cell.cell;
	int			 c, r, g, b;

	(*i)++;
	r = input_get(ictx, *i, 0, -1);
	if (r == -1 || r > 255)
		return;
	(*i)++;
	g = input_get(ictx, *i, 0, -1);
	if (g == -1 || g > 255)
		return;
	(*i)++;
	b = input_get(ictx, *i, 0, -1);
	if (b == -1 || b > 255)
		return;

	c = colour_find_rgb(r, g, b);
	if (fgbg == 38) {
		gc->flags |= GRID_FLAG_FG256;
		gc->fg = c;
	} else if (fgbg == 48) {
		gc->flags |= GRID_FLAG_BG256;
		gc->bg = c;
	}
}