void CSnapIDPool::FreeID(int ID)
{
	if(ID < 0)
		return;
	dbg_assert(m_aIDs[ID].m_State == 1, "id is not alloced");

	m_InUsage--;
	m_aIDs[ID].m_State = 2;
	m_aIDs[ID].m_Timeout = time_get()+time_freq()*5;
	m_aIDs[ID].m_Next = -1;

	if(m_LastTimed != -1)
	{
		m_aIDs[m_LastTimed].m_Next = ID;
		m_LastTimed = ID;
	}
	else
	{
		m_FirstTimed = ID;
		m_LastTimed = ID;
	}
}