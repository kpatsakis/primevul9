void CClient::GetServerInfo(CServerInfo *pServerInfo)
{
	mem_copy(pServerInfo, &m_CurrentServerInfo, sizeof(m_CurrentServerInfo));
}