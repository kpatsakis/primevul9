input_csi_dispatch_sgr(struct input_ctx *ictx)
{
	struct grid_cell	*gc = &ictx->cell.cell;
	u_int			 i;
	int			 n;

	if (ictx->param_list_len == 0) {
		memcpy(gc, &grid_default_cell, sizeof *gc);
		return;
	}

	for (i = 0; i < ictx->param_list_len; i++) {
		if (ictx->param_list[i].type == INPUT_STRING) {
			input_csi_dispatch_sgr_colon(ictx, i);
			continue;
		}
		n = input_get(ictx, i, 0, 0);
		if (n == -1)
			continue;

		if (n == 38 || n == 48 || n == 58) {
			i++;
			switch (input_get(ictx, i, 0, -1)) {
			case 2:
				input_csi_dispatch_sgr_rgb(ictx, n, &i);
				break;
			case 5:
				input_csi_dispatch_sgr_256(ictx, n, &i);
				break;
			}
			continue;
		}

		switch (n) {
		case 0:
			memcpy(gc, &grid_default_cell, sizeof *gc);
			break;
		case 1:
			gc->attr |= GRID_ATTR_BRIGHT;
			break;
		case 2:
			gc->attr |= GRID_ATTR_DIM;
			break;
		case 3:
			gc->attr |= GRID_ATTR_ITALICS;
			break;
		case 4:
			gc->attr &= ~GRID_ATTR_ALL_UNDERSCORE;
			gc->attr |= GRID_ATTR_UNDERSCORE;
			break;
		case 5:
			gc->attr |= GRID_ATTR_BLINK;
			break;
		case 7:
			gc->attr |= GRID_ATTR_REVERSE;
			break;
		case 8:
			gc->attr |= GRID_ATTR_HIDDEN;
			break;
		case 9:
			gc->attr |= GRID_ATTR_STRIKETHROUGH;
			break;
		case 22:
			gc->attr &= ~(GRID_ATTR_BRIGHT|GRID_ATTR_DIM);
			break;
		case 23:
			gc->attr &= ~GRID_ATTR_ITALICS;
			break;
		case 24:
			gc->attr &= ~GRID_ATTR_ALL_UNDERSCORE;
			break;
		case 25:
			gc->attr &= ~GRID_ATTR_BLINK;
			break;
		case 27:
			gc->attr &= ~GRID_ATTR_REVERSE;
			break;
		case 28:
			gc->attr &= ~GRID_ATTR_HIDDEN;
			break;
		case 29:
			gc->attr &= ~GRID_ATTR_STRIKETHROUGH;
			break;
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
			gc->fg = n - 30;
			break;
		case 39:
			gc->fg = 8;
			break;
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			gc->bg = n - 40;
			break;
		case 49:
			gc->bg = 8;
			break;
		case 53:
			gc->attr |= GRID_ATTR_OVERLINE;
			break;
		case 55:
			gc->attr &= ~GRID_ATTR_OVERLINE;
			break;
		case 59:
			gc->us = 0;
			break;
		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:
			gc->fg = n;
			break;
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
			gc->bg = n - 10;
			break;
		}
	}
}