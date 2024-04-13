input_csi_dispatch_sgr_256(struct input_ctx *ictx, int fgbg, u_int *i)
{
	struct grid_cell	*gc = &ictx->cell.cell;
	int			 c;

	(*i)++;
	c = input_get(ictx, *i, 0, -1);
	if (c == -1) {
		if (fgbg == 38) {
			gc->flags &= ~GRID_FLAG_FG256;
			gc->fg = 8;
		} else if (fgbg == 48) {
			gc->flags &= ~GRID_FLAG_BG256;
			gc->bg = 8;
		}
	} else {
		if (fgbg == 38) {
			gc->flags |= GRID_FLAG_FG256;
			gc->fg = c;
		} else if (fgbg == 48) {
			gc->flags |= GRID_FLAG_BG256;
			gc->bg = c;
		}
	}
}