relpTcpSetRemHost(relpTcp_t *pThis, struct sockaddr *pAddr)
{
	relpEngine_t *pEngine;
	int error;
	unsigned char szIP[NI_MAXHOST] = "";
	unsigned char szHname[NI_MAXHOST] = "";
	struct addrinfo hints, *res;
	size_t len;
	
	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);
	pEngine = pThis->pEngine;
	assert(pAddr != NULL);

	error = getnameinfo(pAddr, SALEN(pAddr), (char*)szIP, sizeof(szIP), NULL, 0, NI_NUMERICHOST);
	if(error) {
		pThis->pEngine->dbgprint("Malformed from address %s\n", gai_strerror(error));
		strcpy((char*)szHname, "???");
		strcpy((char*)szIP, "???");
		ABORT_FINALIZE(RELP_RET_INVALID_HNAME);
	}

	if(pEngine->bEnableDns) {
		error = getnameinfo(pAddr, SALEN(pAddr), (char*)szHname, sizeof(szHname), NULL, 0, NI_NAMEREQD);
		if(error == 0) {
			memset (&hints, 0, sizeof (struct addrinfo));
			hints.ai_flags = AI_NUMERICHOST;
			hints.ai_socktype = SOCK_STREAM;
			/* we now do a lookup once again. This one should fail,
			 * because we should not have obtained a non-numeric address. If
			 * we got a numeric one, someone messed with DNS!
			 */
			if(getaddrinfo((char*)szHname, NULL, &hints, &res) == 0) {
				freeaddrinfo (res);
				/* OK, we know we have evil, so let's indicate this to our caller */
				snprintf((char*)szHname, NI_MAXHOST, "[MALICIOUS:IP=%s]", szIP);
				pEngine->dbgprint("Malicious PTR record, IP = \"%s\" HOST = \"%s\"", szIP, szHname);
				iRet = RELP_RET_MALICIOUS_HNAME;
			}
		} else {
			strcpy((char*)szHname, (char*)szIP);
		}
	} else {
		strcpy((char*)szHname, (char*)szIP);
	}

	/* We now have the names, so now let's allocate memory and store them permanently.
	 * (side note: we may hold on to these values for quite a while, thus we trim their
	 * memory consumption)
	 */
	len = strlen((char*)szIP) + 1; /* +1 for \0 byte */
	if((pThis->pRemHostIP = malloc(len)) == NULL)
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	memcpy(pThis->pRemHostIP, szIP, len);

	len = strlen((char*)szHname) + 1; /* +1 for \0 byte */
	if((pThis->pRemHostName = malloc(len)) == NULL) {
		free(pThis->pRemHostIP); /* prevent leak */
		pThis->pRemHostIP = NULL;
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}
	memcpy(pThis->pRemHostName, szHname, len);

finalize_it:
	LEAVE_RELPFUNC;
}