BGD_DECLARE(void) gdImageBmp(gdImagePtr im, FILE *outFile, int compression)
{
	gdIOCtx *out = gdNewFileCtx(outFile);
	if (out == NULL) return;
	gdImageBmpCtx(im, out, compression);
	out->gd_free(out);
}