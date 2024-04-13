bool CClient::ConnectionProblems()
{
	return m_NetClient.GotProblems() != 0;
}