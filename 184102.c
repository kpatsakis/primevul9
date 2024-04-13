static rsRetVal setUmask(void __attribute__((unused)) *pVal, int iUmask)
{
	umask(iUmask);
	DBGPRINTF("umask set to 0%3.3o.\n", iUmask);

	return RS_RET_OK;
}