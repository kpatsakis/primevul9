void CClient::SetState(int s)
{
	if(m_State == IClient::STATE_QUITING)
		return;

	int Old = m_State;
	if(g_Config.m_Debug)
	{
		char aBuf[128];
		str_format(aBuf, sizeof(aBuf), "state change. last=%d current=%d", m_State, s);
		m_pConsole->Print(IConsole::OUTPUT_LEVEL_DEBUG, "client", aBuf);
	}
	m_State = s;
	if(Old != s)
		GameClient()->OnStateChange(m_State, Old);
}