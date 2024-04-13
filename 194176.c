rsRetVal MsgSetStructuredData(msg_t *pMsg, char* pszStrucData)
{
	DEFiRet;
	ISOBJ_TYPE_assert(pMsg, msg);
	if(pMsg->pCSStrucData == NULL) {
		/* we need to obtain the object first */
		CHKiRet(rsCStrConstruct(&pMsg->pCSStrucData));
	}
	/* if we reach this point, we have the object */
	iRet = rsCStrSetSzStr(pMsg->pCSStrucData, (uchar*) pszStrucData);

finalize_it:
	RETiRet;
}