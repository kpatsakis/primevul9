GetDataBlock(gdIOCtx *fd, unsigned char *buf, int *ZeroDataBlockP)
{
	int rv, i;

	rv = GetDataBlock_(fd,buf, ZeroDataBlockP);

	if(VERBOSE) {
		printf("[GetDataBlock returning %d",rv);
		if(rv > 0) {
			printf(":");
			for(i = 0; i < rv; i++) {
				printf(" %02x",buf[i]);
			}
		}
		printf("]\n");
	}

	return rv;
}