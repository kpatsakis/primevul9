BGD_DECLARE(gdImagePtr) gdImageCreateFromBmp(FILE * inFile)
{
	gdImagePtr im = 0;
	gdIOCtx *in = gdNewFileCtx(inFile);
	if (in == NULL) return NULL;
	im = gdImageCreateFromBmpCtx(in);
	in->gd_free(in);
	return im;
}