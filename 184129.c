void legacyOptsEnq(uchar *line)
{
	legacyOptsLL_t *pNew;

	pNew = malloc(sizeof(legacyOptsLL_t));
	if(line == NULL)
		pNew->line = NULL;
	else
		pNew->line = (uchar *) strdup((char *) line);
	pNew->next = NULL;

	if(pLegacyOptsLL == NULL)
		pLegacyOptsLL = pNew;
	else {
		legacyOptsLL_t *pThis = pLegacyOptsLL;

		while(pThis->next != NULL)
			pThis = pThis->next;
		pThis->next = pNew;
	}
}