static inline rsRetVal printline(uchar *hname, uchar *hnameIP, uchar *msg, int flags, flowControl_t flowCtlType,
	prop_t *pInputName, struct syslogTime *stTime, time_t ttGenTime)
{
	DEFiRet;
	register uchar *p;
	int pri;
	msg_t *pMsg;
	prop_t *propFromHost = NULL;
	prop_t *propFromHostIP = NULL;

	/* Now it is time to create the message object (rgerhards) */
	if(stTime == NULL) {
		CHKiRet(msgConstruct(&pMsg));
	} else {
		CHKiRet(msgConstructWithTime(&pMsg, stTime, ttGenTime));
	}
	if(pInputName != NULL)
		MsgSetInputName(pMsg, pInputName);
	MsgSetFlowControlType(pMsg, flowCtlType);
	MsgSetRawMsgWOSize(pMsg, (char*)msg);
	
	/* test for special codes */
	pri = DEFUPRI;
	p = msg;
	if (*p == '<') {
		pri = 0;
		while (isdigit((int) *++p))
		{
		   pri = 10 * pri + (*p - '0');
		}
		if (*p == '>')
			++p;
	}
	if (pri &~ (LOG_FACMASK|LOG_PRIMASK))
		pri = DEFUPRI;
	pMsg->iFacility = LOG_FAC(pri);
	pMsg->iSeverity = LOG_PRI(pri);

	/* Now we look at the HOSTNAME. That is a bit complicated...
	 * If we have a locally received message, it does NOT
	 * contain any hostname information in the message itself.
	 * As such, the HOSTNAME is the same as the system that
	 * the message was received from (that, for obvious reasons,
	 * being the local host).  rgerhards 2004-11-16
	 */
	if((pMsg->msgFlags & PARSE_HOSTNAME) == 0)
		MsgSetHOSTNAME(pMsg, hname, ustrlen(hname));
	MsgSetRcvFromStr(pMsg, hname, ustrlen(hname), &propFromHost);
	CHKiRet(MsgSetRcvFromIPStr(pMsg, hnameIP, ustrlen(hnameIP), &propFromHostIP));
	MsgSetAfterPRIOffs(pMsg, p - msg);
	prop.Destruct(&propFromHost);
	prop.Destruct(&propFromHostIP);

	logmsg(pMsg, flags);

finalize_it:
	RETiRet;
}