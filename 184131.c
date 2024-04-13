parseAndSubmitMessage(uchar *hname, uchar *hnameIP, uchar *msg, int len, int flags, flowControl_t flowCtlType,
	prop_t *pInputName, struct syslogTime *stTime, time_t ttGenTime)
{
	DEFiRet;
	register int iMsg;
	uchar *pMsg;
	uchar *pData;
	uchar *pEnd;
	int iMaxLine;
	uchar *tmpline = NULL;
#	ifdef USE_NETZIP
	uchar *deflateBuf = NULL;
	uLongf iLenDefBuf;
#	endif

	assert(hname != NULL);
	assert(hnameIP != NULL);
	assert(msg != NULL);
	assert(len >= 0);

	/* we first allocate work buffers large enough to hold the configured maximum
	 * size of a message. Over time, we should change this to a more optimal way, i.e.
	 * by calling the function with the actual length of the message to be parsed.
	 * rgerhards, 2008-09-02
	 *
	 * TODO: optimize buffer handling */
	iMaxLine = glbl.GetMaxLine();
	CHKmalloc(tmpline = malloc(sizeof(uchar) * (iMaxLine + 1)));

	/* we first check if we have a NUL character at the very end of the
	 * message. This seems to be a frequent problem with a number of senders.
	 * So I have now decided to drop these NULs. However, if they are intentional,
	 * that may cause us some problems, e.g. with syslog-sign. On the other hand,
	 * current code always has problems with intentional NULs (as it needs to escape
	 * them to prevent problems with the C string libraries), so that does not
	 * really matter. Just to be on the save side, we'll log destruction of such
	 * NULs in the debug log.
	 * rgerhards, 2007-09-14
	 */
	if(*(msg + len - 1) == '\0') {
		DBGPRINTF("dropped NUL at very end of message\n");
		len--;
	}

	/* then we check if we need to drop trailing LFs, which often make
	 * their way into syslog messages unintentionally. In order to remain
	 * compatible to recent IETF developments, we allow the user to
	 * turn on/off this handling.  rgerhards, 2007-07-23
	 */
	if(bDropTrailingLF && *(msg + len - 1) == '\n') {
		DBGPRINTF("dropped LF at very end of message (DropTrailingLF is set)\n");
		len--;
	}

	iMsg = 0;	/* initialize receiving buffer index */
	pMsg = tmpline; /* set receiving buffer pointer */
	pData = msg;	/* set source buffer pointer */
	pEnd = msg + len; /* this is one off, which is intensional */

#	ifdef USE_NETZIP
	/* we first need to check if we have a compressed record. If so,
	 * we must decompress it.
	 */
	if(len > 0 && *msg == 'z') { /* compressed data present? (do NOT change order if conditions!) */
		/* we have compressed data, so let's deflate it. We support a maximum
		 * message size of iMaxLine. If it is larger, an error message is logged
		 * and the message is dropped. We do NOT try to decompress larger messages
		 * as such might be used for denial of service. It might happen to later
		 * builds that such functionality be added as an optional, operator-configurable
		 * feature.
		 */
		int ret;
		iLenDefBuf = iMaxLine;
		CHKmalloc(deflateBuf = malloc(sizeof(uchar) * (iMaxLine + 1)));
		ret = uncompress((uchar *) deflateBuf, &iLenDefBuf, (uchar *) msg+1, len-1);
		DBGPRINTF("Compressed message uncompressed with status %d, length: new %ld, old %d.\n",
		        ret, (long) iLenDefBuf, len-1);
		/* Now check if the uncompression worked. If not, there is not much we can do. In
		 * that case, we log an error message but ignore the message itself. Storing the
		 * compressed text is dangerous, as it contains control characters. So we do
		 * not do this. If someone would like to have a copy, this code here could be
		 * modified to do a hex-dump of the buffer in question. We do not include
		 * this functionality right now.
		 * rgerhards, 2006-12-07
		 */
		if(ret != Z_OK) {
			errmsg.LogError(0, NO_ERRCODE, "Uncompression of a message failed with return code %d "
			            "- enable debug logging if you need further information. "
				    "Message ignored.", ret);
			FINALIZE; /* unconditional exit, nothing left to do... */
		}
		pData = deflateBuf;
		pEnd = deflateBuf + iLenDefBuf;
	}
#	else /* ifdef USE_NETZIP */
	/* in this case, we still need to check if the message is compressed. If so, we must
	 * tell the user we can not accept it.
	 */
	if(len > 0 && *msg == 'z') {
		errmsg.LogError(0, NO_ERRCODE, "Received a compressed message, but rsyslogd does not have compression "
		         "support enabled. The message will be ignored.");
		FINALIZE;
	}	
#	endif /* ifdef USE_NETZIP */

	while(pData < pEnd) {
		if(iMsg >= iMaxLine) {
			/* emergency, we now need to flush, no matter if
			 * we are at end of message or not...
			 */
			if(iMsg == iMaxLine) {
				*(pMsg + iMsg) = '\0'; /* space *is* reserved for this! */
				printline(hname, hnameIP, tmpline, flags, flowCtlType, pInputName, stTime, ttGenTime);
			} else {
				/* This case in theory never can happen. If it happens, we have
				 * a logic error. I am checking for it, because if I would not,
				 * we would address memory invalidly with the code above. I
				 * do not care much about this case, just a debug log entry
				 * (I couldn't do any more smart things anyway...).
				 * rgerhards, 2007-9-20
				 */
				DBGPRINTF("internal error: iMsg > max msg size in parseAndSubmitMessage()\n");
			}
			FINALIZE; /* in this case, we are done... nothing left we can do */
		}
		if(*pData == '\0') { /* guard against \0 characters... */
			/* changed to the sequence (somewhat) proposed in
			 * draft-ietf-syslog-protocol-19. rgerhards, 2006-11-30
			 */
			if(iMsg + 3 < iMaxLine) { /* do we have space? */
				*(pMsg + iMsg++) =  cCCEscapeChar;
				*(pMsg + iMsg++) = '0';
				*(pMsg + iMsg++) = '0';
				*(pMsg + iMsg++) = '0';
			} /* if we do not have space, we simply ignore the '\0'... */
			  /* log an error? Very questionable... rgerhards, 2006-11-30 */
			  /* decided: we do not log an error, it won't help... rger, 2007-06-21 */
			++pData;
		} else if(bEscapeCCOnRcv && iscntrl((int) *pData)) {
			/* we are configured to escape control characters. Please note
			 * that this most probably break non-western character sets like
			 * Japanese, Korean or Chinese. rgerhards, 2007-07-17
			 * Note: sysklogd logs octal values only for DEL and CCs above 127.
			 * For others, it logs ^n where n is the control char converted to an
			 * alphabet character. We like consistency and thus escape it to octal
			 * in all cases. If someone complains, we may change the mode. At least
			 * we known now what's going on.
			 * rgerhards, 2007-07-17
			 */
			if(iMsg + 3 < iMaxLine) { /* do we have space? */
				*(pMsg + iMsg++) = cCCEscapeChar;
				*(pMsg + iMsg++) = '0' + ((*pData & 0300) >> 6);
				*(pMsg + iMsg++) = '0' + ((*pData & 0070) >> 3);
				*(pMsg + iMsg++) = '0' + ((*pData & 0007));
			} /* again, if we do not have space, we ignore the char - see comment at '\0' */
			++pData;
		} else {
			*(pMsg + iMsg++) = *pData++;
		}
	}

	*(pMsg + iMsg) = '\0'; /* space *is* reserved for this! */

	/* typically, we should end up here! */
	printline(hname, hnameIP, tmpline, flags, flowCtlType, pInputName, stTime, ttGenTime);

finalize_it:
	if(tmpline != NULL)
		free(tmpline);
#	ifdef USE_NETZIP
	if(deflateBuf != NULL)
		free(deflateBuf);
#	endif
	RETiRet;
}