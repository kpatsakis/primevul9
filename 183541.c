BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpPtr(int size, void *data)
{
	gdImagePtr im;
	gdIOCtx *in = gdNewDynamicCtxEx(size, data, 0);
	if (in == NULL) return NULL;
	im = gdImageCreateFromBmpCtx(in);
	in->gd_free(in);
	return im;
}