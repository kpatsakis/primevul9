bufOptAdd(char opt, char *arg)
{
	DEFiRet;
	bufOpt_t *pBuf;

	if((pBuf = malloc(sizeof(bufOpt_t))) == NULL)
		ABORT_FINALIZE(RS_RET_OUT_OF_MEMORY);

	pBuf->optchar = opt;
	pBuf->arg = arg;
	pBuf->pNext = NULL;

	if(bufOptLast == NULL) {
		bufOptRoot = pBuf; /* then there is also no root! */
	} else {
		bufOptLast->pNext = pBuf;
	}
	bufOptLast = pBuf;

finalize_it:
	RETiRet;
}