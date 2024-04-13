gdImagePtr gdImageRotateGeneric(gdImagePtr src, const float degrees, const int bgColor)
{
	float _angle = ((float) (-degrees / 180.0f) * (float)M_PI);
	const int src_w  = gdImageSX(src);
	const int src_h = gdImageSY(src);
	const unsigned int new_width = (unsigned int)(abs((int)(src_w * cos(_angle))) + abs((int)(src_h * sin(_angle))) + 0.5f);
	const unsigned int new_height = (unsigned int)(abs((int)(src_w * sin(_angle))) + abs((int)(src_h * cos(_angle))) + 0.5f);
	const gdFixed f_0_5 = gd_ftofx(0.5f);
	const gdFixed f_H = gd_itofx(src_h/2);
	const gdFixed f_W = gd_itofx(src_w/2);
	const gdFixed f_cos = gd_ftofx(cos(-_angle));
	const gdFixed f_sin = gd_ftofx(sin(-_angle));

	unsigned int dst_offset_x;
	unsigned int dst_offset_y = 0;
	unsigned int i;
	gdImagePtr dst;

	const gdFixed f_slop_y = f_sin;
	const gdFixed f_slop_x = f_cos;
	const gdFixed f_slop = f_slop_x > 0 && f_slop_x > 0 ?
							f_slop_x > f_slop_y ? gd_divfx(f_slop_y, f_slop_x) : gd_divfx(f_slop_x, f_slop_y)
						: 0;


	dst = gdImageCreateTrueColor(new_width, new_height);
	if (!dst) {
		return NULL;
	}
	dst->saveAlphaFlag = 1;

	for (i = 0; i < new_height; i++) {
		unsigned int j;
		dst_offset_x = 0;
		for (j = 0; j < new_width; j++) {
			gdFixed f_i = gd_itofx((int)i - (int)new_height/ 2);
			gdFixed f_j = gd_itofx((int)j - (int)new_width / 2);
			gdFixed f_m = gd_mulfx(f_j,f_sin) + gd_mulfx(f_i,f_cos) + f_0_5 + f_H;
			gdFixed f_n = gd_mulfx(f_j,f_cos) - gd_mulfx(f_i,f_sin) + f_0_5 + f_W;
			long m = gd_fxtoi(f_m);
			long n = gd_fxtoi(f_n);

			if ((n <= 0) || (m <= 0) || (m >= src_h) || (n >= src_w)) {
				dst->tpixels[dst_offset_y][dst_offset_x++] = bgColor;
			} else if ((n <= 1) || (m <= 1) || (m >= src_h - 1) || (n >= src_w - 1)) {
				gdFixed f_127 = gd_itofx(127);
				register int c = getPixelInterpolated(src, n, m, bgColor);
				c = c | (( gdTrueColorGetAlpha(c) + ((int)(127* gd_fxtof(f_slop)))) << 24);

				dst->tpixels[dst_offset_y][dst_offset_x++] = _color_blend(bgColor, c);
			} else {
				dst->tpixels[dst_offset_y][dst_offset_x++] = getPixelInterpolated(src, n, m, bgColor);
			}
		}
		dst_offset_y++;
	}
	return dst;
}