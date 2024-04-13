BGD_DECLARE(void) gdImagePngCtx (gdImagePtr im, gdIOCtx * outfile)
{
	/* 2.0.13: 'return' here was an error, thanks to Kevin Smith */
	gdImagePngCtxEx (im, outfile, -1);
}