BGD_DECLARE(void) gdImageFilledPolygon (gdImagePtr im, gdPointPtr p, int n, int c)
{
	int i;
	int j;
	int index;
	int y;
	int miny, maxy, pmaxy;
	int x1, y1;
	int x2, y2;
	int ind1, ind2;
	int ints;
	int fill_color;
	if (n <= 0) {
		return;
	}

	if (c == gdAntiAliased) {
		fill_color = im->AA_color;
	} else {
		fill_color = c;
	}
	if (!im->polyAllocated) {
		if (overflow2(sizeof (int), n)) {
			return;
		}
		im->polyInts = (int *) gdMalloc (sizeof (int) * n);
		if (!im->polyInts) {
			return;
		}
		im->polyAllocated = n;
	}
	if (im->polyAllocated < n) {
		while (im->polyAllocated < n) {
			im->polyAllocated *= 2;
		}
		if (overflow2(sizeof (int), im->polyAllocated)) {
			return;
		}
		im->polyInts = (int *) gdReallocEx (im->polyInts,
						    sizeof (int) * im->polyAllocated);
		if (!im->polyInts) {
			return;
		}
	}
	miny = p[0].y;
	maxy = p[0].y;
	for (i = 1; (i < n); i++) {
		if (p[i].y < miny) {
			miny = p[i].y;
		}
		if (p[i].y > maxy) {
			maxy = p[i].y;
		}
	}
	pmaxy = maxy;
	/* 2.0.16: Optimization by Ilia Chipitsine -- don't waste time offscreen */
	/* 2.0.26: clipping rectangle is even better */
	if (miny < im->cy1) {
		miny = im->cy1;
	}
	if (maxy > im->cy2) {
		maxy = im->cy2;
	}
	/* Fix in 1.3: count a vertex only once */
	for (y = miny; (y <= maxy); y++) {
		ints = 0;
		for (i = 0; (i < n); i++) {
			if (!i) {
				ind1 = n - 1;
				ind2 = 0;
			} else {
				ind1 = i - 1;
				ind2 = i;
			}
			y1 = p[ind1].y;
			y2 = p[ind2].y;
			if (y1 < y2) {
				x1 = p[ind1].x;
				x2 = p[ind2].x;
			} else if (y1 > y2) {
				y2 = p[ind1].y;
				y1 = p[ind2].y;
				x2 = p[ind1].x;
				x1 = p[ind2].x;
			} else {
				continue;
			}

			/* Do the following math as float intermediately, and round to ensure
			 * that Polygon and FilledPolygon for the same set of points have the
			 * same footprint. */

			if ((y >= y1) && (y < y2)) {
				im->polyInts[ints++] = (int) ((float) ((y - y1) * (x2 - x1)) /
				                              (float) (y2 - y1) + 0.5 + x1);
			} else if ((y == pmaxy) && (y == y2)) {
				im->polyInts[ints++] = x2;
			}
		}
		/*
		  2.0.26: polygons pretty much always have less than 100 points,
		  and most of the time they have considerably less. For such trivial
		  cases, insertion sort is a good choice. Also a good choice for
		  future implementations that may wish to indirect through a table.
		*/
		for (i = 1; (i < ints); i++) {
			index = im->polyInts[i];
			j = i;
			while ((j > 0) && (im->polyInts[j - 1] > index)) {
				im->polyInts[j] = im->polyInts[j - 1];
				j--;
			}
			im->polyInts[j] = index;
		}
		for (i = 0; (i < (ints-1)); i += 2) {
			/* 2.0.29: back to gdImageLine to prevent segfaults when
			  performing a pattern fill */
			gdImageLine (im, im->polyInts[i], y, im->polyInts[i + 1], y,
			             fill_color);
		}
	}
	/* If we are drawing this AA, then redraw the border with AA lines. */
	/* This doesn't work as well as I'd like, but it doesn't clash either. */
	if (c == gdAntiAliased) {
		gdImagePolygon (im, p, n, c);
	}
}