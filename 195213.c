	if((pRcvBuf = malloc(MAXLINE * sizeof(char))) == NULL) {
		ABORT_FINALIZE(RS_RET_OUT_OF_MEMORY);
	}