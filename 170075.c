static void OneLine32(struct bmp_progressive_state *context)
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
		int a_lshift, a_rshift;

		r_lshift = 8 - context->r_bits;
		g_lshift = 8 - context->g_bits;
		b_lshift = 8 - context->b_bits;
		a_lshift = 8 - context->a_bits;

		r_rshift = context->r_bits - r_lshift;
		g_rshift = context->g_bits - g_lshift;
		b_rshift = context->b_bits - b_lshift;
		a_rshift = context->a_bits - a_lshift;

		for (i = 0; i < context->Header.width; i++) {
			unsigned int v, r, g, b, a;

			v = src[0] | (src[1] << 8) | (src[2] << 16) | (src[3] << 24);

			r = (v & context->r_mask) >> context->r_shift;
			g = (v & context->g_mask) >> context->g_shift;
			b = (v & context->b_mask) >> context->b_shift;
			a = (v & context->a_mask) >> context->a_shift;

			*pixels++ = (r << r_lshift) | (r >> r_rshift);
			*pixels++ = (g << g_lshift) | (g >> g_rshift);
			*pixels++ = (b << b_lshift) | (b >> b_rshift);
                        if (context->a_bits)
			  *pixels++ = (a << a_lshift) | (a >> a_rshift);
                        else
                          *pixels++ = 0xff;

			src += 4;
		}
	} else
		for (i = 0; i < context->Header.width; i++) {
			*pixels++ = src[2];
			*pixels++ = src[1];
			*pixels++ = src[0];
			*pixels++ = 0xff;

			src += 4;
		}
}