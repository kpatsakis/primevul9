static int bmp_read_palette(gdImagePtr im, gdIOCtxPtr infile, int count, int read_four)
{
	int i;
	int r, g, b, z;

	for (i = 0; i < count; i++) {
		if (
		    !gdGetByte(&b, infile) ||
		    !gdGetByte(&g, infile) ||
		    !gdGetByte(&r, infile) ||
		    (read_four && !gdGetByte(&z, infile))
		) {
			return 1;
		}
		im->red[i] = r;
		im->green[i] = g;
		im->blue[i] = b;
		im->open[i] = 1;
	}
	return 0;
}