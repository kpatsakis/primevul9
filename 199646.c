void CDCCBounce::ReachedMaxBuffer() {
	DEBUG(GetSockName() << " == ReachedMaxBuffer()");

	CString sType = (m_bIsChat) ? "Chat" : "Xfer";

	m_pModule->PutModule("DCC " + sType + " Bounce (" + m_sRemoteNick + "): Too long line received");
	Close();
}