static void OneLine1(struct bmp_progressive_state *context)
{
	gint X;
	guchar *Pixels;

	X = 0;
	if (context->Header.Negative == 0)
		Pixels = (context->pixbuf->pixels +
			  context->pixbuf->rowstride *
			  (context->Header.height - context->Lines - 1));
	else
		Pixels = (context->pixbuf->pixels +
			  context->pixbuf->rowstride *
			  context->Lines);
	while (X < context->Header.width) {
		gint Bit;

		Bit = (context->buff[X / 8]) >> (7 - (X & 7));
		Bit = Bit & 1;
		Pixels[X * 3 + 0] = context->Colormap[Bit][2];
		Pixels[X * 3 + 1] = context->Colormap[Bit][1];
		Pixels[X * 3 + 2] = context->Colormap[Bit][0];
		X++;
	}
}