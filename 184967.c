CClient::CClient() : m_DemoPlayer(&m_SnapshotDelta), m_DemoRecorder(&m_SnapshotDelta)
{
	m_pEditor = 0;
	m_pInput = 0;
	m_pGraphics = 0;
	m_pSound = 0;
	m_pGameClient = 0;
	m_pMap = 0;
	m_pConsole = 0;

	m_RenderFrameTime = 0.0001f;
	m_RenderFrameTimeLow = 1.0f;
	m_RenderFrameTimeHigh = 0.0f;
	m_RenderFrames = 0;
	m_LastRenderTime = time_get();

	m_GameTickSpeed = SERVER_TICK_SPEED;

	m_WindowMustRefocus = 0;
	m_SnapCrcErrors = 0;
	m_AutoScreenshotRecycle = false;
	m_EditorActive = false;

	m_AckGameTick = -1;
	m_CurrentRecvTick = 0;
	m_RconAuthed = 0;

	// version-checking
	m_aVersionStr[0] = '0';
	m_aVersionStr[1] = 0;

	// pinging
	m_PingStartTime = 0;

	//
	m_aCurrentMap[0] = 0;
	m_CurrentMapCrc = 0;

	//
	m_aCmdConnect[0] = 0;

	// map download
	m_aMapdownloadFilename[0] = 0;
	m_aMapdownloadName[0] = 0;
	m_MapdownloadFile = 0;
	m_MapdownloadChunk = 0;
	m_MapdownloadCrc = 0;
	m_MapdownloadAmount = -1;
	m_MapdownloadTotalsize = -1;

	m_CurrentServerInfoRequestTime = -1;

	m_CurrentInput = 0;

	m_State = IClient::STATE_OFFLINE;
	m_aServerAddressStr[0] = 0;

	mem_zero(m_aSnapshots, sizeof(m_aSnapshots));
	m_SnapshotStorage.Init();
	m_RecivedSnapshots = 0;

	m_VersionInfo.m_State = CVersionInfo::STATE_INIT;
}