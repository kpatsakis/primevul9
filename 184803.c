BGD_DECLARE(void *) gdImagePngPtrEx (gdImagePtr im, int *size, int level)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx (2048, NULL);
	if (out == NULL) return NULL;
	if (!_gdImagePngCtxEx (im, out, level)) {
		rv = gdDPExtractData (out, size);
	} else {
		rv = NULL;
	}
	out->gd_free (out);
	return rv;
}