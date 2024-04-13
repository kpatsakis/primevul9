void CDCCBounce::Shutdown() {
	m_pPeer = NULL;
	DEBUG(GetSockName() << " == Close(); because my peer told me to");
	Close();
}