static int bmp_read_info(gdIOCtx *infile, bmp_info_t *info)
{
	/* read BMP length so we can work out the version */
	if (!gdGetIntLSB(&info->len, infile)) {
		return 1;
	}

	switch (info->len) {
		/* For now treat Windows v4 + v5 as v3 */
	case BMP_WINDOWS_V3:
	case BMP_WINDOWS_V4:
	case BMP_WINDOWS_V5:
		BMP_DEBUG(printf("Reading Windows Header\n"));
		if (bmp_read_windows_v3_info(infile, info)) {
			return 1;
		}
		break;
	case BMP_OS2_V1:
		if (bmp_read_os2_v1_info(infile, info)) {
			return 1;
		}
		break;
	case BMP_OS2_V2:
		if (bmp_read_os2_v2_info(infile, info)) {
			return 1;
		}
		break;
	default:
		BMP_DEBUG(printf("Unhandled bitmap\n"));
		return 1;
	}
	return 0;
}