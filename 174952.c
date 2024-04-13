static gdImagePtr gdImageScaleBilinearPalette(gdImagePtr im, const unsigned int new_width, const unsigned int new_height)
{
	long _width = MAX(1, new_width);
	long _height = MAX(1, new_height);
	float dx = (float)gdImageSX(im) / (float)_width;
	float dy = (float)gdImageSY(im) / (float)_height;
	gdFixed f_dx = gd_ftofx(dx);
	gdFixed f_dy = gd_ftofx(dy);
	gdFixed f_1 = gd_itofx(1);

	int dst_offset_h;
	int dst_offset_v = 0;
	long i;
	gdImagePtr new_img;
	const int transparent = im->transparent;

	if (new_width == 0 || new_height == 0) {
		return NULL;
	}

	new_img = gdImageCreateTrueColor(new_width, new_height);
	if (new_img == NULL) {
		return NULL;
	}

	if (transparent < 0) {
		/* uninitialized */
		new_img->transparent = -1;
	} else {
		new_img->transparent = gdTrueColorAlpha(im->red[transparent], im->green[transparent], im->blue[transparent], im->alpha[transparent]);
	}

	for (i=0; i < _height; i++) {
		long j;
		const gdFixed f_i = gd_itofx(i);
		const gdFixed f_a = gd_mulfx(f_i, f_dy);
		register long m = gd_fxtoi(f_a);

		dst_offset_h = 0;

		for (j=0; j < _width; j++) {
			/* Update bitmap */
			gdFixed f_j = gd_itofx(j);
			gdFixed f_b = gd_mulfx(f_j, f_dx);

			const long n = gd_fxtoi(f_b);
			gdFixed f_f = f_a - gd_itofx(m);
			gdFixed f_g = f_b - gd_itofx(n);

			const gdFixed f_w1 = gd_mulfx(f_1-f_f, f_1-f_g);
			const gdFixed f_w2 = gd_mulfx(f_1-f_f, f_g);
			const gdFixed f_w3 = gd_mulfx(f_f, f_1-f_g);
			const gdFixed f_w4 = gd_mulfx(f_f, f_g);
			unsigned int pixel1;
			unsigned int pixel2;
			unsigned int pixel3;
			unsigned int pixel4;
			register gdFixed f_r1, f_r2, f_r3, f_r4,
					f_g1, f_g2, f_g3, f_g4,
					f_b1, f_b2, f_b3, f_b4,
					f_a1, f_a2, f_a3, f_a4;

			/* zero for the background color, nothig gets outside anyway */
			pixel1 = getPixelOverflowPalette(im, n, m, 0);
			pixel2 = getPixelOverflowPalette(im, n + 1, m, 0);
			pixel3 = getPixelOverflowPalette(im, n, m + 1, 0);
			pixel4 = getPixelOverflowPalette(im, n + 1, m + 1, 0);

			f_r1 = gd_itofx(gdTrueColorGetRed(pixel1));
			f_r2 = gd_itofx(gdTrueColorGetRed(pixel2));
			f_r3 = gd_itofx(gdTrueColorGetRed(pixel3));
			f_r4 = gd_itofx(gdTrueColorGetRed(pixel4));
			f_g1 = gd_itofx(gdTrueColorGetGreen(pixel1));
			f_g2 = gd_itofx(gdTrueColorGetGreen(pixel2));
			f_g3 = gd_itofx(gdTrueColorGetGreen(pixel3));
			f_g4 = gd_itofx(gdTrueColorGetGreen(pixel4));
			f_b1 = gd_itofx(gdTrueColorGetBlue(pixel1));
			f_b2 = gd_itofx(gdTrueColorGetBlue(pixel2));
			f_b3 = gd_itofx(gdTrueColorGetBlue(pixel3));
			f_b4 = gd_itofx(gdTrueColorGetBlue(pixel4));
			f_a1 = gd_itofx(gdTrueColorGetAlpha(pixel1));
			f_a2 = gd_itofx(gdTrueColorGetAlpha(pixel2));
			f_a3 = gd_itofx(gdTrueColorGetAlpha(pixel3));
			f_a4 = gd_itofx(gdTrueColorGetAlpha(pixel4));

			{
				const unsigned char red = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_r1) + gd_mulfx(f_w2, f_r2) + gd_mulfx(f_w3, f_r3) + gd_mulfx(f_w4, f_r4));
				const unsigned char green = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_g1) + gd_mulfx(f_w2, f_g2) + gd_mulfx(f_w3, f_g3) + gd_mulfx(f_w4, f_g4));
				const unsigned char blue = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_b1) + gd_mulfx(f_w2, f_b2) + gd_mulfx(f_w3, f_b3) + gd_mulfx(f_w4, f_b4));
				const unsigned char alpha = (unsigned char) gd_fxtoi(gd_mulfx(f_w1, f_a1) + gd_mulfx(f_w2, f_a2) + gd_mulfx(f_w3, f_a3) + gd_mulfx(f_w4, f_a4));

				new_img->tpixels[dst_offset_v][dst_offset_h] = gdTrueColorAlpha(red, green, blue, alpha);
			}

			dst_offset_h++;
		}

		dst_offset_v++;
	}
	return new_img;
}