void CClient::InitInterfaces()
{
	// fetch interfaces
	m_pEngine = Kernel()->RequestInterface<IEngine>();
	m_pEditor = Kernel()->RequestInterface<IEditor>();
	//m_pGraphics = Kernel()->RequestInterface<IEngineGraphics>();
	m_pSound = Kernel()->RequestInterface<IEngineSound>();
	m_pGameClient = Kernel()->RequestInterface<IGameClient>();
	m_pInput = Kernel()->RequestInterface<IEngineInput>();
	m_pMap = Kernel()->RequestInterface<IEngineMap>();
	m_pMasterServer = Kernel()->RequestInterface<IEngineMasterServer>();
	m_pStorage = Kernel()->RequestInterface<IStorage>();

	//
	m_ServerBrowser.SetBaseInfo(&m_ContactClient, m_pGameClient->NetVersion());
	m_Friends.Init();
}