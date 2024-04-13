bufOptRemove(int *opt, char **arg)
{
	DEFiRet;
	bufOpt_t *pBuf;

	if(bufOptRoot == NULL)
		ABORT_FINALIZE(RS_RET_END_OF_LINKEDLIST);
	pBuf = bufOptRoot;

	*opt = pBuf->optchar;
	*arg = pBuf->arg;

	bufOptRoot = pBuf->pNext;
	free(pBuf);

finalize_it:
	RETiRet;
}