LWZReadByte(gdIOCtx *fd, LZW_STATIC_DATA *sd, char flag, int input_code_size, int *ZeroDataBlockP)
{
	int rv;

	rv = LWZReadByte_(fd, sd, flag, input_code_size, ZeroDataBlockP);

	if(VERBOSE) {
		printf("[LWZReadByte(,%d,%d) returning %d]\n",flag,input_code_size,rv);
	}

	return rv;
}