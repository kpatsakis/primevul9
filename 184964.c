void CClient::DemoRecorder_Start(const char *pFilename, bool WithTimestamp)
{
	if(State() != IClient::STATE_ONLINE)
		m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "demorec/record", "client is not online");
	else
	{
		char aFilename[128];
		if(WithTimestamp)
		{
			char aDate[20];
			str_timestamp(aDate, sizeof(aDate));
			str_format(aFilename, sizeof(aFilename), "demos/%s_%s.demo", pFilename, aDate);
		}
		else
			str_format(aFilename, sizeof(aFilename), "demos/%s.demo", pFilename);
		m_DemoRecorder.Start(Storage(), m_pConsole, aFilename, GameClient()->NetVersion(), m_aCurrentMap, m_CurrentMapCrc, "client");
	}
}