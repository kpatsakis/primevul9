void CDCCBounce::ConnectionRefused() {
	DEBUG(GetSockName() << " == ConnectionRefused()");

	CString sType = (m_bIsChat) ? "Chat" : "Xfer";
	CString sHost = Csock::GetHostName();
	if (!sHost.empty()) {
		sHost = " to [" + sHost + " " + CString(Csock::GetPort()) + "]";
	} else {
		sHost = ".";
	}

	m_pModule->PutModule("DCC " + sType + " Bounce (" + m_sRemoteNick + "): Connection Refused while connecting" + sHost);
}