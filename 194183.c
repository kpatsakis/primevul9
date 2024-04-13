msgGetMsgVar(msg_t *pThis, cstr_t *pstrPropName, var_t **ppVar)
{
	DEFiRet;
	var_t *pVar;
	size_t propLen;
	uchar *pszProp = NULL;
	cstr_t *pstrProp;
	propid_t propid;
	unsigned short bMustBeFreed = 0;

	ISOBJ_TYPE_assert(pThis, msg);
	ASSERT(pstrPropName != NULL);
	ASSERT(ppVar != NULL);

	/* make sure we have a var_t instance */
	CHKiRet(var.Construct(&pVar));
	CHKiRet(var.ConstructFinalize(pVar));

	/* always call MsgGetProp() without a template specifier */
	/* TODO: optimize propNameToID() call -- rgerhards, 2009-06-26 */
	propNameToID(pstrPropName, &propid);
	pszProp = (uchar*) MsgGetProp(pThis, NULL, propid, &propLen, &bMustBeFreed);

	/* now create a string object out of it and hand that over to the var */
	CHKiRet(rsCStrConstructFromszStr(&pstrProp, pszProp));
	CHKiRet(var.SetString(pVar, pstrProp));

	/* finally store var */
	*ppVar = pVar;

finalize_it:
	if(bMustBeFreed)
		free(pszProp);

	RETiRet;
}