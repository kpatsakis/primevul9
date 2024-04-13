BGD_DECLARE(gdImagePtr) gdImageCreateFromGif(FILE *fdFile)
{
	gdIOCtx *fd = gdNewFileCtx(fdFile);
	gdImagePtr im;

	if (fd == NULL) return NULL;
	im = gdImageCreateFromGifCtx(fd);

	fd->gd_free(fd);

	return im;
}