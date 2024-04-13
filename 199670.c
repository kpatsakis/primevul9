void CDCCBounce::SockError(int iErrno) {
	DEBUG(GetSockName() << " == SockError(" << iErrno << ")");
	CString sType = (m_bIsChat) ? "Chat" : "Xfer";

	if (IsRemote()) {
		CString sHost = Csock::GetHostName();
		if (!sHost.empty()) {
			sHost = "[" + sHost + " " + CString(Csock::GetPort()) + "]";
		}

		m_pModule->PutModule("DCC " + sType + " Bounce (" + m_sRemoteNick + "): Socket error [" + CString(strerror(iErrno)) + "]" + sHost);
	} else {
		m_pModule->PutModule("DCC " + sType + " Bounce (" + m_sRemoteNick + "): Socket error [" + CString(strerror(iErrno)) + "] [" + Csock::GetLocalIP() + ":" + CString(Csock::GetLocalPort()) + "]");
	}
}