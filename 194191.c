resolveDNS(msg_t *pMsg) {
	rsRetVal localRet;
	prop_t *propFromHost = NULL;
	prop_t *propFromHostIP = NULL;
	uchar fromHost[NI_MAXHOST];
	uchar fromHostIP[NI_MAXHOST];
	uchar fromHostFQDN[NI_MAXHOST];
	DEFiRet;

	CHKiRet(objUse(net, CORE_COMPONENT));
	if(pMsg->msgFlags & NEEDS_DNSRESOL) {
		localRet = net.cvthname(pMsg->rcvFrom.pfrominet, fromHost, fromHostFQDN, fromHostIP);
		if(localRet == RS_RET_OK) {
			MsgSetRcvFromStr(pMsg, fromHost, ustrlen(fromHost), &propFromHost);
			CHKiRet(MsgSetRcvFromIPStr(pMsg, fromHostIP, ustrlen(fromHostIP), &propFromHostIP));
		}
	}
finalize_it:
	if(iRet != RS_RET_OK) {
		/* best we can do: remove property */
		MsgSetRcvFromStr(pMsg, UCHAR_CONSTANT(""), 0, &propFromHost);
		prop.Destruct(&propFromHost);
	}
	if(propFromHost != NULL)
		prop.Destruct(&propFromHost);
	if(propFromHostIP != NULL)
		prop.Destruct(&propFromHostIP);
	RETiRet;
}