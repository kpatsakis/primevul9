CDCCBounce::CDCCBounce(CBounceDCCMod* pMod, unsigned long uLongIP, unsigned short uPort,
		const CString& sFileName, const CString& sRemoteNick,
		const CString& sRemoteIP, bool bIsChat) : CSocket(pMod) {
	m_uRemotePort = uPort;
	m_sConnectIP = CUtils::GetIP(uLongIP);
	m_sRemoteIP = sRemoteIP;
	m_sFileName = sFileName;
	m_sRemoteNick = sRemoteNick;
	m_pModule = pMod;
	m_bIsChat = bIsChat;
	m_sLocalIP = pMod->GetLocalDCCIP();
	m_pPeer = NULL;
	m_bIsRemote = false;

	if (bIsChat) {
		EnableReadLine();
	} else {
		DisableReadLine();
	}
}