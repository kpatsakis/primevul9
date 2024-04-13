static void OneLine16(struct bmp_progressive_state *context)
{
	int i;
	guchar *pixels;
	guchar *src;

	if (!context->Header.Negative)
		pixels = (context->pixbuf->pixels +
			  context->pixbuf->rowstride * (context->Header.height - context->Lines - 1));
	else
		pixels = (context->pixbuf->pixels +
			  context->pixbuf->rowstride * context->Lines);

	src = context->buff;

	if (context->Compressed == BI_BITFIELDS) {
		int r_lshift, r_rshift;
		int g_lshift, g_rshift;
		int b_lshift, b_rshift;

		r_lshift = 8 - context->r_bits;
		g_lshift = 8 - context->g_bits;
		b_lshift = 8 - context->b_bits;

		r_rshift = context->r_bits - r_lshift;
		g_rshift = context->g_bits - g_lshift;
		b_rshift = context->b_bits - b_lshift;

		for (i = 0; i < context->Header.width; i++) {
			int v, r, g, b;

			v = (int) src[0] | ((int) src[1] << 8);

			r = (v & context->r_mask) >> context->r_shift;
			g = (v & context->g_mask) >> context->g_shift;
			b = (v & context->b_mask) >> context->b_shift;

			*pixels++ = (r << r_lshift) | (r >> r_rshift);
			*pixels++ = (g << g_lshift) | (g >> g_rshift);
			*pixels++ = (b << b_lshift) | (b >> b_rshift);

			src += 2;
		}
	} else
		for (i = 0; i < context->Header.width; i++) {
			int v, r, g, b;

			v = src[0] | (src[1] << 8);

			r = (v >> 10) & 0x1f;
			g = (v >> 5) & 0x1f;
			b = v & 0x1f;

			*pixels++ = (r << 3) | (r >> 2);
			*pixels++ = (g << 3) | (g >> 2);
			*pixels++ = (b << 3) | (b >> 2);

			src += 2;
		}
}