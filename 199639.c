Csock* CDCCBounce::GetSockObj(const CString& sHost, unsigned short uPort) {
	Close();

	if (m_sRemoteIP.empty()) {
		m_sRemoteIP = sHost;
	}

	CDCCBounce* pSock = new CDCCBounce(m_pModule, sHost, uPort, m_sRemoteNick, m_sRemoteIP, m_sFileName, m_bIsChat);
	CDCCBounce* pRemoteSock = new CDCCBounce(m_pModule, sHost, uPort, m_sRemoteNick, m_sRemoteIP, m_sFileName, m_bIsChat);
	pSock->SetPeer(pRemoteSock);
	pRemoteSock->SetPeer(pSock);
	pRemoteSock->SetRemote(true);
	pSock->SetRemote(false);

	if (!CZNC::Get().GetManager().Connect(m_sConnectIP, m_uRemotePort, "DCC::" + CString((m_bIsChat) ? "Chat" : "XFER") + "::Remote::" + m_sRemoteNick, 60, false, m_sLocalIP, pRemoteSock)) {
		pRemoteSock->Close();
	}

	pSock->SetSockName(GetSockName());
	return pSock;
}