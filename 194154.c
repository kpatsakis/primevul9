msgSetFromSockinfo(msg_t *pThis, struct sockaddr_storage *sa){ 
	DEFiRet;
	assert(pThis->rcvFrom.pRcvFrom == NULL);

	CHKmalloc(pThis->rcvFrom.pfrominet = malloc(sizeof(struct sockaddr_storage)));
	memcpy(pThis->rcvFrom.pfrominet, sa, sizeof(struct sockaddr_storage));

finalize_it:
	RETiRet;
}