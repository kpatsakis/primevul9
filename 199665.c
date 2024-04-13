void CDCCBounce::ReadPaused() {
	if (!m_pPeer || m_pPeer->GetInternalWriteBuffer().length() <= m_uiMinDCCBuffer)
		UnPauseRead();
}