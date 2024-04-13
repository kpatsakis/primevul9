void CDCCBounce::PutPeer(const CString& sLine) {
	if (m_pPeer) {
		m_pPeer->PutServ(sLine);
	} else {
		PutServ("*** Not connected yet ***");
	}
}