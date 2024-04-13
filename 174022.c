relpTcpLstnInit(relpTcp_t *pThis, unsigned char *pLstnPort, int ai_family)
{
	struct addrinfo hints, *res, *r;
	int error, maxs, *s, on = 1;
	int sockflags;
	unsigned char *pLstnPt;

	ENTER_RELPFUNC;
	RELPOBJ_assert(pThis, Tcp);

	pLstnPt = pLstnPort;
	assert(pLstnPt != NULL);
	
	pThis->pEngine->dbgprint("creating relp tcp listen socket on port %s\n", pLstnPt);

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = ai_family;
	hints.ai_socktype = SOCK_STREAM;

	error = getaddrinfo(NULL, (char*) pLstnPt, &hints, &res);
	if(error) {
		pThis->pEngine->dbgprint("error %d querying port '%s'\n", error, pLstnPt);
		ABORT_FINALIZE(RELP_RET_INVALID_PORT);
	}

	/* Count max number of sockets we may open */
	for(maxs = 0, r = res; r != NULL ; r = r->ai_next, maxs++)
		/* EMPTY */;
	pThis->socks = malloc((maxs+1) * sizeof(int));
	if (pThis->socks == NULL) {
	pThis->pEngine->dbgprint("couldn't allocate memory for TCP listen sockets, suspending RELP message reception.");
		freeaddrinfo(res);
		ABORT_FINALIZE(RELP_RET_OUT_OF_MEMORY);
	}

	*pThis->socks = 0;   /* num of sockets counter at start of array */
	s = pThis->socks + 1;
	for(r = res; r != NULL ; r = r->ai_next) {
		*s = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if (*s < 0) {
			if(!(r->ai_family == PF_INET6 && errno == EAFNOSUPPORT))
				pThis->pEngine->dbgprint("creating relp tcp listen socket");
				/* it is debatable if PF_INET with EAFNOSUPPORT should
				 * also be ignored...
				 */
			continue;
		}

#ifdef IPV6_V6ONLY
		if (r->ai_family == AF_INET6) {
			int iOn = 1;
			if (setsockopt(*s, IPPROTO_IPV6, IPV6_V6ONLY,
				(char *)&iOn, sizeof (iOn)) < 0) {
				close(*s);
				*s = -1;
				continue;
			}
		}
#endif
		if(setsockopt(*s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) < 0 ) {
			pThis->pEngine->dbgprint("error %d setting relp/tcp socket option\n", errno);
			close(*s);
			*s = -1;
			continue;
		}

		/* We use non-blocking IO! */
		if((sockflags = fcntl(*s, F_GETFL)) != -1) {
			sockflags |= O_NONBLOCK;
			/* SETFL could fail too, so get it caught by the subsequent
			 * error check.
			 */
			sockflags = fcntl(*s, F_SETFL, sockflags);
		}
		if(sockflags == -1) {
			pThis->pEngine->dbgprint("error %d setting fcntl(O_NONBLOCK) on relp socket", errno);
			close(*s);
			*s = -1;
			continue;
		}

#ifdef ENABLE_TLS
		if(pThis->bEnableTLS) {
			CHKRet(relpTcpLstnInitTLS(pThis));
		}
#endif /* #ifdef ENABLE_TLS */

	        if( (bind(*s, r->ai_addr, r->ai_addrlen) < 0)
#ifndef IPV6_V6ONLY
		     && (errno != EADDRINUSE)
#endif
	           ) {
			char msgbuf[4096];
			snprintf(msgbuf, sizeof(msgbuf), "error while binding relp tcp socket "
				 "on port '%s'", pLstnPort);
			msgbuf[sizeof(msgbuf)-1] = '\0';
			callOnErr(pThis, msgbuf, errno);
			close(*s);
			*s = -1;
			continue;
		}

		if(listen(*s,pThis->iSessMax / 10 + 5) < 0) {
			/* If the listen fails, it most probably fails because we ask
			 * for a too-large backlog. So in this case we first set back
			 * to a fixed, reasonable, limit that should work. Only if
			 * that fails, too, we give up.
			 */
			pThis->pEngine->dbgprint("listen with a backlog of %d failed - retrying with default of 32.",
				    pThis->iSessMax / 10 + 5);
			if(listen(*s, 32) < 0) {
				pThis->pEngine->dbgprint("relp listen error %d, suspending\n", errno);
	                	close(*s);
				*s = -1;
				continue;
			}
		}

		(*pThis->socks)++;
		s++;
	}

	if(res != NULL)
		freeaddrinfo(res);

	if(*pThis->socks != maxs)
		pThis->pEngine->dbgprint("We could initialize %d RELP TCP listen sockets out of %d we received "
		 	"- this may or may not be an error indication.\n", *pThis->socks, maxs);

	if(*pThis->socks == 0) {
		pThis->pEngine->dbgprint("No RELP TCP listen socket could successfully be initialized, "
			 "message reception via RELP disabled.\n");
		free(pThis->socks);
		ABORT_FINALIZE(RELP_RET_COULD_NOT_BIND);
	}

finalize_it:
	LEAVE_RELPFUNC;
}