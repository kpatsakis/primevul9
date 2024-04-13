freeAllDynMemForTermination(void)
{
	free(pszMainMsgQFName);
	free(pModDir);
	free(pszConfDAGFile);
}