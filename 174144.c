GetDataBlock_(gdIOCtx *fd, unsigned char *buf, int *ZeroDataBlockP)
{
	unsigned char count;

	if(!ReadOK(fd, &count, 1)) {
		return -1;
	}

	*ZeroDataBlockP = count == 0;

	if((count != 0) && (!ReadOK(fd, buf, count))) {
		return -1;
	}

	return count;
}