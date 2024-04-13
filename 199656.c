void CDCCBounce::ReadData(const char* data, size_t len) {
	if (m_pPeer) {
		m_pPeer->Write(data, len);

		size_t BufLen = m_pPeer->GetInternalWriteBuffer().length();

		if (BufLen >= m_uiMaxDCCBuffer) {
			DEBUG(GetSockName() << " The send buffer is over the "
					"limit (" << BufLen <<"), throttling");
			PauseRead();
		}
	}
}