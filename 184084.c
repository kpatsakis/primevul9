logmsg(msg_t *pMsg, int flags)
{
	char *msg;

	BEGINfunc
	assert(pMsg != NULL);
	assert(pMsg->pszRawMsg != NULL);

	msg = (char*) pMsg->pszRawMsg + pMsg->offAfterPRI;  /* point to start of text, after PRI */
	DBGPRINTF("logmsg: flags %x, from '%s', msg %s\n", flags, getRcvFrom(pMsg), msg);

	/* rger 2005-11-24 (happy thanksgiving!): we now need to check if we have
	 * a traditional syslog message or one formatted according to syslog-protocol.
	 * We need to apply different parsers depending on that. We use the
	 * -protocol VERSION field for the detection.
	 */
	if(msg[0] == '1' && msg[1] == ' ') {
		DBGPRINTF("Message has syslog-protocol format.\n");
		setProtocolVersion(pMsg, 1);
		if(parseRFCSyslogMsg(pMsg, flags) == 1) {
			msgDestruct(&pMsg);
			return;
		}
	} else { /* we have legacy syslog */
		DBGPRINTF("Message has legacy syslog format.\n");
		setProtocolVersion(pMsg, 0);
		if(parseLegacySyslogMsg(pMsg, flags) == 1) {
			msgDestruct(&pMsg);
			return;
		}
	}

	/* ---------------------- END PARSING ---------------- */
	
	/* now submit the message to the main queue - then we are done */
	pMsg->msgFlags = flags;
	MsgPrepareEnqueue(pMsg);
	qqueueEnqObj(pMsgQueue, pMsg->flowCtlType, (void*) pMsg);
	ENDfunc
}