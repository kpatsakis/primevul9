	if(pData->fdErrFile != -1) {
		close(pData->fdErrFile);
		pData->fdErrFile = -1;
	}