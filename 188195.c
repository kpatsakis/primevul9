BGD_DECLARE(void) gdImageCopyResampled (gdImagePtr dst,
										gdImagePtr src,
										int dstX, int dstY,
										int srcX, int srcY,
										int dstW, int dstH, int srcW, int srcH)
{
	int x, y;
	double sy1, sy2, sx1, sx2;
	if (!dst->trueColor) {
		gdImageCopyResized (dst, src, dstX, dstY, srcX, srcY, dstW, dstH,
		                    srcW, srcH);
		return;
	}
	for (y = dstY; (y < dstY + dstH); y++) {
		sy1 = ((double) y - (double) dstY) * (double) srcH / (double) dstH;
		sy2 = ((double) (y + 1) - (double) dstY) * (double) srcH /
		      (double) dstH;
		for (x = dstX; (x < dstX + dstW); x++) {
			double sx, sy;
			double spixels = 0;
			double red = 0.0, green = 0.0, blue = 0.0, alpha = 0.0;
			double alpha_sum = 0.0, contrib_sum = 0.0;

			sx1 = ((double) x - (double) dstX) * (double) srcW / dstW;
			sx2 = ((double) (x + 1) - (double) dstX) * (double) srcW / dstW;
			sy = sy1;
			do {
				double yportion;
				if (floor2 (sy) == floor2 (sy1)) {
					yportion = 1.0 - (sy - floor2 (sy));
					if (yportion > sy2 - sy1) {
						yportion = sy2 - sy1;
					}
					sy = floor2 (sy);
				} else if (sy == floor2 (sy2)) {
					yportion = sy2 - floor2 (sy2);
				} else {
					yportion = 1.0;
				}
				sx = sx1;
				do {
					double xportion;
					double pcontribution;
					int p;
					if (floor2 (sx) == floor2 (sx1)) {
						xportion = 1.0 - (sx - floor2 (sx));
						if (xportion > sx2 - sx1) {
							xportion = sx2 - sx1;
						}
						sx = floor2 (sx);
					} else if (sx == floor2 (sx2)) {
						xportion = sx2 - floor2 (sx2);
					} else {
						xportion = 1.0;
					}
					pcontribution = xportion * yportion;
					/* 2.08: previously srcX and srcY were ignored.
					   Andrew Pattison */
					p = gdImageGetTrueColorPixel (src,
					                              (int) sx + srcX,
					                              (int) sy + srcY);
					red += gdTrueColorGetRed (p) * pcontribution;
					green += gdTrueColorGetGreen (p) * pcontribution;
					blue += gdTrueColorGetBlue (p) * pcontribution;
					alpha += gdTrueColorGetAlpha (p) * pcontribution;
					spixels += xportion * yportion;
					sx += 1.0;
				} while (sx < sx2);
				sy += 1.0;
			} while (sy < sy2);
			if (spixels != 0.0) {
				red /= spixels;
				green /= spixels;
				blue /= spixels;
				alpha /= spixels;
				alpha += 0.5;
			}
			if ( alpha_sum != 0.0f) {
				if( contrib_sum != 0.0f) {
					alpha_sum /= contrib_sum;
				}
				red /= alpha_sum;
				green /= alpha_sum;
				blue /= alpha_sum;
			}
			/* Clamping to allow for rounding errors above */
			if (red > 255.0) {
				red = 255.0;
			}
			if (green > 255.0) {
				green = 255.0;
			}
			if (blue > 255.0) {
				blue = 255.0;
			}
			if (alpha > gdAlphaMax) {
				alpha = gdAlphaMax;
			}
			gdImageSetPixel (dst,
			                 x, y,
			                 gdTrueColorAlpha ((int) red,
			                                   (int) green,
			                                   (int) blue, (int) alpha));
		}
	}
}