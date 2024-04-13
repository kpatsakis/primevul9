BGD_DECLARE(void) gdImagePngEx (gdImagePtr im, FILE * outFile, int level)
{
	gdIOCtx *out = gdNewFileCtx (outFile);
	if (out == NULL) return;
	gdImagePngCtxEx (im, out, level);
	out->gd_free (out);
}