BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpCtx(gdIOCtxPtr infile)
{
	bmp_hdr_t *hdr;
	bmp_info_t *info;
	gdImagePtr im = NULL;
	int error = 0;

	if (!(hdr= (bmp_hdr_t *)gdCalloc(1, sizeof(bmp_hdr_t)))) {
		return NULL;
	}

	if (bmp_read_header(infile, hdr)) {
		gdFree(hdr);
		return NULL;
	}

	if (hdr->magic != 0x4d42) {
		gdFree(hdr);
		return NULL;
	}

	if (!(info = (bmp_info_t *)gdCalloc(1, sizeof(bmp_info_t)))) {
		gdFree(hdr);
		return NULL;
	}

	if (bmp_read_info(infile, info)) {
		gdFree(hdr);
		gdFree(info);
		return NULL;
	}

	BMP_DEBUG(printf("Numcolours: %d\n", info->numcolors));
	BMP_DEBUG(printf("Width: %d\n", info->width));
	BMP_DEBUG(printf("Height: %d\n", info->height));
	BMP_DEBUG(printf("Planes: %d\n", info->numplanes));
	BMP_DEBUG(printf("Depth: %d\n", info->depth));
	BMP_DEBUG(printf("Offset: %d\n", hdr->off));

	if (info->depth >= 16) {
		im = gdImageCreateTrueColor(info->width, info->height);
	} else {
		im = gdImageCreate(info->width, info->height);
	}

	if (!im) {
		gdFree(hdr);
		gdFree(info);
		return NULL;
	}

	switch (info->depth) {
	case 1:
		BMP_DEBUG(printf("1-bit image\n"));
		error = bmp_read_1bit(im, infile, info, hdr);
		break;
	case 4:
		BMP_DEBUG(printf("4-bit image\n"));
		error = bmp_read_4bit(im, infile, info, hdr);
		break;
	case 8:
		BMP_DEBUG(printf("8-bit image\n"));
		error = bmp_read_8bit(im, infile, info, hdr);
		break;
	case 16:
	case 24:
	case 32:
		BMP_DEBUG(printf("Direct BMP image\n"));
		error = bmp_read_direct(im, infile, info, hdr);
		break;
	default:
		BMP_DEBUG(printf("Unknown bit count\n"));
		error = 1;
	}

	gdFree(hdr);
	gdFree(info);

	if (error) {
		gdImageDestroy(im);
		return NULL;
	}

	return im;
}