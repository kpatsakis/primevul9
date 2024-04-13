BGD_DECLARE(gdImagePtr) gdImageCreateFromPng (FILE * inFile)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewFileCtx (inFile);
	if (in == NULL) return NULL;
	im = gdImageCreateFromPngCtx (in);
	in->gd_free (in);
	return im;
}