BGD_DECLARE(void) gdImageRectangle (gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int thick = im->thick;

	if (x1 == x2 && y1 == y2 && thick == 1) {
		gdImageSetPixel(im, x1, y1, color);
		return;
	}

	if (y2 < y1) {
		int t;
		t = y1;
		y1 = y2;
		y2 = t;

		t = x1;
		x1 = x2;
		x2 = t;
	}

	if (thick > 1) {
		int cx, cy, x1ul, y1ul, x2lr, y2lr;
		int half = thick >> 1;
		x1ul = x1 - half;
		y1ul = y1 - half;

		x2lr = x2 + half;
		y2lr = y2 + half;

		cy = y1ul + thick;
		while (cy-- > y1ul) {
			cx = x1ul - 1;
			while (cx++ < x2lr) {
				gdImageSetPixel(im, cx, cy, color);
			}
		}

		cy = y2lr - thick;
		while (cy++ < y2lr) {
			cx = x1ul - 1;
			while (cx++ < x2lr) {
				gdImageSetPixel(im, cx, cy, color);
			}
		}

		cy = y1ul + thick - 1;
		while (cy++ < y2lr -thick) {
			cx = x1ul - 1;
			while (cx++ < x1ul + thick) {
				gdImageSetPixel(im, cx, cy, color);
			}
		}

		cy = y1ul + thick - 1;
		while (cy++ < y2lr -thick) {
			cx = x2lr - thick - 1;
			while (cx++ < x2lr) {
				gdImageSetPixel(im, cx, cy, color);
			}
		}

		return;
	} else {
		gdImageLine(im, x1, y1, x2, y1, color);
		gdImageLine(im, x1, y2, x2, y2, color);
		gdImageLine(im, x1, y1 + 1, x1, y2 - 1, color);
		gdImageLine(im, x2, y1 + 1, x2, y2 - 1, color);
	}
}