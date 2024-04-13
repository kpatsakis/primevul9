CDCCBounce::CDCCBounce(CBounceDCCMod* pMod, const CString& sHostname, unsigned short uPort,
		const CString& sRemoteNick, const CString& sRemoteIP, const CString& sFileName,
		int iTimeout, bool bIsChat) : CSocket(pMod, sHostname, uPort, iTimeout) {
	m_uRemotePort = 0;
	m_bIsChat = bIsChat;
	m_pModule = pMod;
	m_pPeer = NULL;
	m_sRemoteNick = sRemoteNick;
	m_sFileName = sFileName;
	m_sRemoteIP = sRemoteIP;
	m_bIsRemote = false;

	SetMaxBufferThreshold(10240);
	if (bIsChat) {
		EnableReadLine();
	} else {
		DisableReadLine();
	}
}