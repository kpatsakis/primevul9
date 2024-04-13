BGD_DECLARE(void) gdImageColorTransparent (gdImagePtr im, int color)
{
	if (!im->trueColor) {
		if((color < -1) || (color >= gdMaxColors)) {
			return;
		}
		if (im->transparent != -1) {
			im->alpha[im->transparent] = gdAlphaOpaque;
		}
		if (color != -1) {
			im->alpha[color] = gdAlphaTransparent;
		}
	}
	im->transparent = color;
}