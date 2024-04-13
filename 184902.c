void CClient::ServerInfoRequest()
{
	mem_zero(&m_CurrentServerInfo, sizeof(m_CurrentServerInfo));
	m_CurrentServerInfoRequestTime = 0;
}