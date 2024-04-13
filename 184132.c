void legacyOptsFree(void)
{
	legacyOptsLL_t *pThis = pLegacyOptsLL, *pNext;

	while(pThis != NULL) {
		if(pThis->line != NULL)
			free(pThis->line);
		pNext = pThis->next;
		free(pThis);
		pThis = pNext;
	}
}