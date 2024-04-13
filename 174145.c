ReadColorMap(gdIOCtx *fd, int number, unsigned char (*buffer)[256])
{
	int i;
	unsigned char rgb[3];

	for(i = 0; i < number; ++i) {
		if(!ReadOK(fd, rgb, sizeof(rgb))) {
			return TRUE;
		}

		buffer[CM_RED][i] = rgb[0];
		buffer[CM_GREEN][i] = rgb[1];
		buffer[CM_BLUE][i] = rgb[2];
	}

	return FALSE;
}