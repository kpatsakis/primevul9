rsRetVal MsgSetProperty(msg_t *pThis, var_t *pProp)
{
	prop_t *myProp;
	prop_t *propRcvFrom = NULL;
	prop_t *propRcvFromIP = NULL;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, msg);
	assert(pProp != NULL);

 	if(isProp("iProtocolVersion")) {
		setProtocolVersion(pThis, pProp->val.num);
 	} else if(isProp("iSeverity")) {
		pThis->iSeverity = pProp->val.num;
 	} else if(isProp("iFacility")) {
		pThis->iFacility = pProp->val.num;
 	} else if(isProp("msgFlags")) {
		pThis->msgFlags = pProp->val.num;
 	} else if(isProp("offMSG")) {
		MsgSetMSGoffs(pThis, pProp->val.num);
	} else if(isProp("pszRawMsg")) {
		MsgSetRawMsg(pThis, (char*) rsCStrGetSzStrNoNULL(pProp->val.pStr), cstrLen(pProp->val.pStr));
 	/* enable this, if someone actually uses UxTradMsg, delete after some  time has
	 * passed and nobody complained -- rgerhards, 2009-06-16
	} else if(isProp("offAfterPRI")) {
		pThis->offAfterPRI = pProp->val.num;
	*/
	} else if(isProp("pszUxTradMsg")) {
		/*IGNORE*/; /* this *was* a property, but does no longer exist */
	} else if(isProp("pszTAG")) {
		MsgSetTAG(pThis, rsCStrGetSzStrNoNULL(pProp->val.pStr), cstrLen(pProp->val.pStr));
	} else if(isProp("pszInputName")) {
		/* we need to create a property */ 
		CHKiRet(prop.Construct(&myProp));
		CHKiRet(prop.SetString(myProp, rsCStrGetSzStrNoNULL(pProp->val.pStr), rsCStrLen(pProp->val.pStr)));
		CHKiRet(prop.ConstructFinalize(myProp));
		MsgSetInputName(pThis, myProp);
		prop.Destruct(&myProp);
	} else if(isProp("pszRcvFromIP")) {
		MsgSetRcvFromIPStr(pThis, rsCStrGetSzStrNoNULL(pProp->val.pStr), rsCStrLen(pProp->val.pStr), &propRcvFromIP);
		prop.Destruct(&propRcvFromIP);
	} else if(isProp("pszRcvFrom")) {
		MsgSetRcvFromStr(pThis, rsCStrGetSzStrNoNULL(pProp->val.pStr), rsCStrLen(pProp->val.pStr), &propRcvFrom);
		prop.Destruct(&propRcvFrom);
	} else if(isProp("pszHOSTNAME")) {
		MsgSetHOSTNAME(pThis, rsCStrGetSzStrNoNULL(pProp->val.pStr), rsCStrLen(pProp->val.pStr));
	} else if(isProp("pCSStrucData")) {
		MsgSetStructuredData(pThis, (char*) rsCStrGetSzStrNoNULL(pProp->val.pStr));
	} else if(isProp("pCSAPPNAME")) {
		MsgSetAPPNAME(pThis, (char*) rsCStrGetSzStrNoNULL(pProp->val.pStr));
	} else if(isProp("pCSPROCID")) {
		MsgSetPROCID(pThis, (char*) rsCStrGetSzStrNoNULL(pProp->val.pStr));
	} else if(isProp("pCSMSGID")) {
		MsgSetMSGID(pThis, (char*) rsCStrGetSzStrNoNULL(pProp->val.pStr));
 	} else if(isProp("ttGenTime")) {
		pThis->ttGenTime = pProp->val.num;
	} else if(isProp("tRcvdAt")) {
		memcpy(&pThis->tRcvdAt, &pProp->val.vSyslogTime, sizeof(struct syslogTime));
	} else if(isProp("tTIMESTAMP")) {
		memcpy(&pThis->tTIMESTAMP, &pProp->val.vSyslogTime, sizeof(struct syslogTime));
	} else if(isProp("pszMSG")) {
		dbgprintf("no longer supported property pszMSG silently ignored\n");
	}

finalize_it:
	RETiRet;
}