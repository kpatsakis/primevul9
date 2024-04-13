BGD_DECLARE(void) gdImagePng (gdImagePtr im, FILE * outFile)
{
	gdIOCtx *out = gdNewFileCtx (outFile);
	if (out == NULL) return;
	gdImagePngCtxEx (im, out, -1);
	out->gd_free (out);
}