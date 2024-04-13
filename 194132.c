rsRetVal propNameToID(cstr_t *pCSPropName, propid_t *pPropID)
{
	uchar *pName;
	DEFiRet;

	assert(pCSPropName != NULL);
	assert(pPropID != NULL);
	pName = rsCStrGetSzStrNoNULL(pCSPropName);

	/* sometimes there are aliases to the original MonitoWare
	 * property names. These come after || in the ifs below. */
	if(!strcmp((char*) pName, "msg")) {
		*pPropID = PROP_MSG;
	} else if(!strcmp((char*) pName, "timestamp")
		  || !strcmp((char*) pName, "timereported")) {
		*pPropID = PROP_TIMESTAMP;
	} else if(!strcmp((char*) pName, "hostname") || !strcmp((char*) pName, "source")) {
		*pPropID = PROP_HOSTNAME;
	} else if(!strcmp((char*) pName, "syslogtag")) {
		*pPropID = PROP_SYSLOGTAG;
	} else if(!strcmp((char*) pName, "rawmsg")) {
		*pPropID = PROP_RAWMSG;
	/* enable this, if someone actually uses UxTradMsg, delete after some  time has
	 * passed and nobody complained -- rgerhards, 2009-06-16
	} else if(!strcmp((char*) pName, "uxtradmsg")) {
		pRes = getUxTradMsg(pMsg);
	*/
	} else if(!strcmp((char*) pName, "inputname")) {
		*pPropID = PROP_INPUTNAME;
	} else if(!strcmp((char*) pName, "fromhost")) {
		*pPropID = PROP_FROMHOST;
	} else if(!strcmp((char*) pName, "fromhost-ip")) {
		*pPropID = PROP_FROMHOST_IP;
	} else if(!strcmp((char*) pName, "pri")) {
		*pPropID = PROP_PRI;
	} else if(!strcmp((char*) pName, "pri-text")) {
		*pPropID = PROP_PRI_TEXT;
	} else if(!strcmp((char*) pName, "iut")) {
		*pPropID = PROP_IUT;
	} else if(!strcmp((char*) pName, "syslogfacility")) {
		*pPropID = PROP_SYSLOGFACILITY;
	} else if(!strcmp((char*) pName, "syslogfacility-text")) {
		*pPropID = PROP_SYSLOGFACILITY_TEXT;
	} else if(!strcmp((char*) pName, "syslogseverity") || !strcmp((char*) pName, "syslogpriority")) {
		*pPropID = PROP_SYSLOGSEVERITY;
	} else if(!strcmp((char*) pName, "syslogseverity-text") || !strcmp((char*) pName, "syslogpriority-text")) {
		*pPropID = PROP_SYSLOGSEVERITY_TEXT;
	} else if(!strcmp((char*) pName, "timegenerated")) {
		*pPropID = PROP_TIMEGENERATED;
	} else if(!strcmp((char*) pName, "programname")) {
		*pPropID = PROP_PROGRAMNAME;
	} else if(!strcmp((char*) pName, "protocol-version")) {
		*pPropID = PROP_PROTOCOL_VERSION;
	} else if(!strcmp((char*) pName, "structured-data")) {
		*pPropID = PROP_STRUCTURED_DATA;
	} else if(!strcmp((char*) pName, "app-name")) {
		*pPropID = PROP_APP_NAME;
	} else if(!strcmp((char*) pName, "procid")) {
		*pPropID = PROP_PROCID;
	} else if(!strcmp((char*) pName, "msgid")) {
		*pPropID = PROP_MSGID;
	/* here start system properties (those, that do not relate to the message itself */
	} else if(!strcmp((char*) pName, "$now")) {
		*pPropID = PROP_SYS_NOW;
	} else if(!strcmp((char*) pName, "$year")) {
		*pPropID = PROP_SYS_YEAR;
	} else if(!strcmp((char*) pName, "$month")) {
		*pPropID = PROP_SYS_MONTH;
	} else if(!strcmp((char*) pName, "$day")) {
		*pPropID = PROP_SYS_DAY;
	} else if(!strcmp((char*) pName, "$hour")) {
		*pPropID = PROP_SYS_HOUR;
	} else if(!strcmp((char*) pName, "$hhour")) {
		*pPropID = PROP_SYS_HHOUR;
	} else if(!strcmp((char*) pName, "$qhour")) {
		*pPropID = PROP_SYS_QHOUR;
	} else if(!strcmp((char*) pName, "$minute")) {
		*pPropID = PROP_SYS_MINUTE;
	} else if(!strcmp((char*) pName, "$myhostname")) {
		*pPropID = PROP_SYS_MYHOSTNAME;
	} else {
		*pPropID = PROP_INVALID;
		iRet = RS_RET_VAR_NOT_FOUND;
	}

	RETiRet;
}