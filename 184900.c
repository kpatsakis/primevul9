void CGraph::ScaleMin()
{
	int i = 0;
	m_Min = m_Max;
	for(i = 0; i < MAX_VALUES; i++)
	{
		if(m_aValues[i] < m_Min)
			m_Min = m_aValues[i];
	}
}