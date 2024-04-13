void CDCCBounce::Timeout() {
	DEBUG(GetSockName() << " == Timeout()");
	CString sType = (m_bIsChat) ? "Chat" : "Xfer";

	if (IsRemote()) {
		CString sHost = Csock::GetHostName();
		if (!sHost.empty()) {
			sHost = " to [" + sHost + " " + CString(Csock::GetPort()) + "]";
		} else {
			sHost = ".";
		}

		m_pModule->PutModule("DCC " + sType + " Bounce (" + m_sRemoteNick + "): Timeout while connecting" + sHost);
	} else {
		m_pModule->PutModule("DCC " + sType + " Bounce (" + m_sRemoteNick + "): Timeout waiting for incoming connection [" + Csock::GetLocalIP() + ":" + CString(Csock::GetLocalPort()) + "]");
	}
}