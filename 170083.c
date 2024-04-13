static void OneLine24(struct bmp_progressive_state *context)
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
		Pixels[X * 3 + 0] = context->buff[X * 3 + 2];
		Pixels[X * 3 + 1] = context->buff[X * 3 + 1];
		Pixels[X * 3 + 2] = context->buff[X * 3 + 0];
		X++;
	}

}