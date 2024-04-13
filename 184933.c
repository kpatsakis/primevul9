void CClient::Connect(const char *pAddress)
{
	char aBuf[512];
	int Port = 8303;

	Disconnect();

	str_copy(m_aServerAddressStr, pAddress, sizeof(m_aServerAddressStr));

	str_format(aBuf, sizeof(aBuf), "connecting to '%s'", m_aServerAddressStr);
	m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "client", aBuf);

	ServerInfoRequest();

	if(net_host_lookup(m_aServerAddressStr, &m_ServerAddress, m_NetClient.NetType()) != 0)
	{
		char aBufMsg[256];
		str_format(aBufMsg, sizeof(aBufMsg), "could not find the address of %s, connecting to localhost", aBuf);
		m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "client", aBufMsg);
		net_host_lookup("localhost", &m_ServerAddress, m_NetClient.NetType());
	}

	m_RconAuthed = 0;
	if(m_ServerAddress.port == 0)
		m_ServerAddress.port = Port;
	m_NetClient.Connect(&m_ServerAddress);
	SetState(IClient::STATE_CONNECTING);

	if(m_DemoRecorder.IsRecording())
		DemoRecorder_Stop();

	m_InputtimeMarginGraph.Init(-150.0f, 150.0f);
	m_GametimeMarginGraph.Init(-150.0f, 150.0f);
}