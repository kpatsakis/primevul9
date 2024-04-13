rsRetVal MsgSetAPPNAME(msg_t *pMsg, char* pszAPPNAME)
{
	DEFiRet;
	assert(pMsg != NULL);
	if(pMsg->pCSAPPNAME == NULL) {
		/* we need to obtain the object first */
		CHKiRet(rsCStrConstruct(&pMsg->pCSAPPNAME));
	}
	/* if we reach this point, we have the object */
	iRet = rsCStrSetSzStr(pMsg->pCSAPPNAME, (uchar*) pszAPPNAME);

finalize_it:
	RETiRet;
}