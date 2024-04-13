logmsgInternal(int iErr, int pri, uchar *msg, int flags)
{
	uchar pszTag[33];
	msg_t *pMsg;
	DEFiRet;

	CHKiRet(msgConstruct(&pMsg));
	MsgSetInputName(pMsg, pInternalInputName);
	MsgSetRawMsgWOSize(pMsg, (char*)msg);
	MsgSetHOSTNAME(pMsg, glbl.GetLocalHostName(), ustrlen(glbl.GetLocalHostName()));
	MsgSetRcvFrom(pMsg, glbl.GetLocalHostNameProp());
	MsgSetRcvFromIP(pMsg, pLocalHostIP);
	/* check if we have an error code associated and, if so,
	 * adjust the tag. -- rgerhards, 2008-06-27
	 */
	if(iErr == NO_ERRCODE) {
		MsgSetTAG(pMsg, UCHAR_CONSTANT("rsyslogd:"), sizeof("rsyslogd:") - 1);
	} else {
		size_t len = snprintf((char*)pszTag, sizeof(pszTag), "rsyslogd%d:", iErr);
		pszTag[32] = '\0'; /* just to make sure... */
		MsgSetTAG(pMsg, pszTag, len);
	}
	pMsg->iFacility = LOG_FAC(pri);
	pMsg->iSeverity = LOG_PRI(pri);
	pMsg->bParseHOSTNAME = 0;
	flags |= INTERNAL_MSG;

	/* we now check if we should print internal messages out to stderr. This was
	 * suggested by HKS as a way to help people troubleshoot rsyslog configuration
	 * (by running it interactively. This makes an awful lot of sense, so I add
	 * it here. -- rgerhards, 2008-07-28
	 * Note that error messages can not be disable during a config verify. This
	 * permits us to process unmodified config files which otherwise contain a
	 * supressor statement.
	 */
	if(((Debug || NoFork) && bErrMsgToStderr) || iConfigVerify) {
		if(LOG_PRI(pri) == LOG_ERR)
			fprintf(stderr, "rsyslogd: %s\n", msg);
	}

	if(bHaveMainQueue == 0) { /* not yet in queued mode */
		iminternalAddMsg(pri, pMsg, flags);
	} else {
		/* we have the queue, so we can simply provide the 
		 * message to the queue engine.
		 */
		logmsg(pMsg, flags);
	}
finalize_it:
	RETiRet;
}