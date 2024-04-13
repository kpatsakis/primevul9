CDCCBounce::~CDCCBounce() {
	if (m_pPeer) {
		m_pPeer->Shutdown();
		m_pPeer = NULL;
	}
}