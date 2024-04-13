static void OneLine4(struct bmp_progressive_state *context)
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
		guchar Pix;

		Pix = context->buff[X / 2];

		Pixels[X * 3 + 0] =
		    context->Colormap[Pix >> 4][2];
		Pixels[X * 3 + 1] =
		    context->Colormap[Pix >> 4][1];
		Pixels[X * 3 + 2] =
		    context->Colormap[Pix >> 4][0];
		X++;
		if (X < context->Header.width) {
			/* Handle the other 4 bit pixel only when there is one */
			Pixels[X * 3 + 0] =
			    context->Colormap[Pix & 15][2];
			Pixels[X * 3 + 1] =
			    context->Colormap[Pix & 15][1];
			Pixels[X * 3 + 2] =
			    context->Colormap[Pix & 15][0];
			X++;
		}
	}

}