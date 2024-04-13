static rsRetVal addListner(void __attribute__((unused)) *pVal, uchar *pNewVal)
{
	DEFiRet;
	uchar *bindAddr;
	int *newSocks;
	int *tmpSocks;
	int iSrc, iDst;

	/* check which address to bind to. We could do this more compact, but have not
	 * done so in order to make the code more readable. -- rgerhards, 2007-12-27
	 */
	if(pszBindAddr == NULL)
		bindAddr = NULL;
	else if(pszBindAddr[0] == '*' && pszBindAddr[1] == '\0')
		bindAddr = NULL;
	else
		bindAddr = pszBindAddr;

	dbgprintf("Trying to open syslog UDP ports at %s:%s.\n",
		  (bindAddr == NULL) ? (uchar*)"*" : bindAddr, pNewVal);

	newSocks = net.create_udp_socket(bindAddr, (pNewVal == NULL || *pNewVal == '\0') ? (uchar*) "514" : pNewVal, 1);
	if(newSocks != NULL) {
		/* we now need to add the new sockets to the existing set */
		if(udpLstnSocks == NULL) {
			/* esay, we can just replace it */
			udpLstnSocks = newSocks;
		} else {
			/* we need to add them */
			if((tmpSocks = malloc(sizeof(int) * 1 + newSocks[0] + udpLstnSocks[0])) == NULL) {
				dbgprintf("out of memory trying to allocate udp listen socket array\n");
				/* in this case, we discard the new sockets but continue with what we
				 * already have
				 */
				free(newSocks);
				ABORT_FINALIZE(RS_RET_OUT_OF_MEMORY);
			} else {
				/* ready to copy */
				iDst = 1;
				for(iSrc = 1 ; iSrc <= udpLstnSocks[0] ; ++iSrc)
					tmpSocks[iDst++] = udpLstnSocks[iSrc];
				for(iSrc = 1 ; iSrc <= newSocks[0] ; ++iSrc)
					tmpSocks[iDst++] = newSocks[iSrc];
				tmpSocks[0] = udpLstnSocks[0] + newSocks[0];
				free(newSocks);
				free(udpLstnSocks);
				udpLstnSocks = tmpSocks;
			}
		}
	}

finalize_it:
	free(pNewVal); /* in any case, this is no longer needed */

	RETiRet;
}