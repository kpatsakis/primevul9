bool CServer::IsAuthed(int ClientID)
{
	return m_aClients[ClientID].m_Authed;
}