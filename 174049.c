relpTcpAbortDestruct(relpTcp_t **ppThis)
{
	struct linger ling;

	ENTER_RELPFUNC;
	assert(ppThis != NULL);
	RELPOBJ_assert((*ppThis), Tcp);

	if((*ppThis)->sock != -1) {
		ling.l_onoff = 1;
		ling.l_linger = 0;
		if(setsockopt((*ppThis)->sock, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0 ) {
			(*ppThis)->pEngine->dbgprint("could not set SO_LINGER, errno %d\n", errno);
		}
	}

	iRet = relpTcpDestruct(ppThis);

	LEAVE_RELPFUNC;
}