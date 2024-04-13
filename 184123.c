int parseRFCSyslogMsg(msg_t *pMsg, int flags)
{
	uchar *p2parse;
	uchar *pBuf;
	int lenMsg;
	int bContParse = 1;

	BEGINfunc
	assert(pMsg != NULL);
	assert(pMsg->pszRawMsg != NULL);
	p2parse = pMsg->pszRawMsg + pMsg->offAfterPRI; /* point to start of text, after PRI */
	lenMsg = pMsg->iLenRawMsg - pMsg->offAfterPRI;

	/* do a sanity check on the version and eat it (the caller checked this already) */
	assert(p2parse[0] == '1' && p2parse[1] == ' ');
	p2parse += 2;
	lenMsg -= 2;

	/* Now get us some memory we can use as a work buffer while parsing.
	 * We simply allocated a buffer sufficiently large to hold all of the
	 * message, so we can not run into any troubles. I think this is
	 * more wise then to use individual buffers.
	 */
	if((pBuf = malloc(sizeof(uchar) * (lenMsg + 1))) == NULL)
		return 1;
		
	/* IMPORTANT NOTE:
	 * Validation is not actually done below nor are any errors handled. I have
	 * NOT included this for the current proof of concept. However, it is strongly
	 * advisable to add it when this code actually goes into production.
	 * rgerhards, 2005-11-24
	 */

	/* TIMESTAMP */
	if(datetime.ParseTIMESTAMP3339(&(pMsg->tTIMESTAMP),  &p2parse, &lenMsg) == RS_RET_OK) {
		if(flags & IGNDATE) {
			/* we need to ignore the msg data, so simply copy over reception date */
			memcpy(&pMsg->tTIMESTAMP, &pMsg->tRcvdAt, sizeof(struct syslogTime));
		}
	} else {
		DBGPRINTF("no TIMESTAMP detected!\n");
		bContParse = 0;
	}

	/* HOSTNAME */
	if(bContParse) {
		parseRFCField(&p2parse, pBuf, &lenMsg);
		MsgSetHOSTNAME(pMsg, pBuf, ustrlen(pBuf));
	}

	/* APP-NAME */
	if(bContParse) {
		parseRFCField(&p2parse, pBuf, &lenMsg);
		MsgSetAPPNAME(pMsg, (char*)pBuf);
	}

	/* PROCID */
	if(bContParse) {
		parseRFCField(&p2parse, pBuf, &lenMsg);
		MsgSetPROCID(pMsg, (char*)pBuf);
	}

	/* MSGID */
	if(bContParse) {
		parseRFCField(&p2parse, pBuf, &lenMsg);
		MsgSetMSGID(pMsg, (char*)pBuf);
	}

	/* STRUCTURED-DATA */
	if(bContParse) {
		parseRFCStructuredData(&p2parse, pBuf, &lenMsg);
		MsgSetStructuredData(pMsg, (char*)pBuf);
	}

	/* MSG */
	MsgSetMSGoffs(pMsg, p2parse - pMsg->pszRawMsg);

	free(pBuf);
	ENDfunc
	return 0; /* all ok */
}