static int bmp_read_os2_v1_info(gdIOCtxPtr infile, bmp_info_t *info)
{
	if (
	    !gdGetWordLSB((signed short int *)&info->width, infile) ||
	    !gdGetWordLSB((signed short int *)&info->height, infile) ||
	    !gdGetWordLSB(&info->numplanes, infile) ||
	    !gdGetWordLSB(&info->depth, infile)
	) {
		return 1;
	}

	/* OS2 v1 doesn't support topdown */
	info->topdown = 0;

	/* The spec says the depth can only be a few value values. */
	if (info->depth != 1 && info->depth != 4 && info->depth != 8 &&
	        info->depth != 16 && info->depth != 24) {
		return 1;
	}

	info->numcolors = 1 << info->depth;
	info->type = BMP_PALETTE_3;

	if (info->width <= 0 || info->height <= 0 || info->numplanes <= 0) {
		return 1;
	}

	return 0;
}