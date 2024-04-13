rsRetVal MsgSetPROCID(msg_t *pMsg, char* pszPROCID)
{
	DEFiRet;
	ISOBJ_TYPE_assert(pMsg, msg);
	if(pMsg->pCSPROCID == NULL) {
		/* we need to obtain the object first */
		CHKiRet(cstrConstruct(&pMsg->pCSPROCID));
	}
	/* if we reach this point, we have the object */
	CHKiRet(rsCStrSetSzStr(pMsg->pCSPROCID, (uchar*) pszPROCID));
	CHKiRet(cstrFinalize(pMsg->pCSPROCID));

finalize_it:
	RETiRet;
}