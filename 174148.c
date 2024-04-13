GetCode(gdIOCtx *fd, CODE_STATIC_DATA *scd, int code_size, int flag, int *ZeroDataBlockP)
{
	int rv;

	rv = GetCode_(fd, scd, code_size,flag, ZeroDataBlockP);

	if(VERBOSE) {
		printf("[GetCode(,%d,%d) returning %d]\n",code_size,flag,rv);
	}

	return rv;
}