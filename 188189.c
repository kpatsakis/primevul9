BGD_DECLARE(void) gdImageFilledRectangle (gdImagePtr im, int x1, int y1, int x2, int y2,
		int color)
{
	int x, y;

	if (x1 == x2 && y1 == y2) {
		gdImageSetPixel(im, x1, y1, color);
		return;
	}

	if (x1 > x2) {
		x = x1;
		x1 = x2;
		x2 = x;
	}

	if (y1 > y2) {
		y = y1;
		y1 = y2;
		y2 = y;
	}

	if (x1 < 0) {
		x1 = 0;
	}

	if (x2 >= gdImageSX(im)) {
		x2 = gdImageSX(im) - 1;
	}

	if (y1 < 0) {
		y1 = 0;
	}

	if (y2 >= gdImageSY(im)) {
		y2 = gdImageSY(im) - 1;
	}

	for (y = y1; (y <= y2); y++) {
		for (x = x1; (x <= x2); x++) {
			gdImageSetPixel (im, x, y, color);
		}
	}
}