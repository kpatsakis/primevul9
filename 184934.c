int CClient::PlayerScoreComp(const void *a, const void *b)
{
	CServerInfo::CClient *p0 = (CServerInfo::CClient *)a;
	CServerInfo::CClient *p1 = (CServerInfo::CClient *)b;
	if(p0->m_Player && !p1->m_Player)
		return -1;
	if(!p0->m_Player && p1->m_Player)
		return 1;
	if(p0->m_Score == p1->m_Score)
		return 0;
	if(p0->m_Score < p1->m_Score)
		return 1;
	return -1;
}