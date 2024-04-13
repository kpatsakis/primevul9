DoExtension(gdIOCtx *fd, int label, int *Transparent, int *ZeroDataBlockP)
{
	unsigned char buf[256];

	switch(label) {
	case 0xf9: /* Graphic Control Extension */
		memset(buf, 0, 4); /* initialize a few bytes in the case the next function fails */
		(void) GetDataBlock(fd, (unsigned char*) buf, ZeroDataBlockP);
#if 0
		Gif89.disposal  = (buf[0] >> 2) & 0x7;
		Gif89.inputFlag = (buf[0] >> 1) & 0x1;
		Gif89.delayTime = LM_to_uint(buf[1], buf[2]);
#endif
		if((buf[0] & 0x1) != 0) {
			*Transparent = buf[3];
		}

		while(GetDataBlock(fd, (unsigned char*) buf, ZeroDataBlockP) > 0);

		return FALSE;

	default:
		break;
	}

	while(GetDataBlock(fd, (unsigned char*) buf, ZeroDataBlockP) > 0);

	return FALSE;
}