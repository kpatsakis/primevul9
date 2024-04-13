void CClient::OnEnterGame()
{
	// reset input
	int i;
	for(i = 0; i < 200; i++)
		m_aInputs[i].m_Tick = -1;
	m_CurrentInput = 0;

	// reset snapshots
	m_aSnapshots[SNAP_CURRENT] = 0;
	m_aSnapshots[SNAP_PREV] = 0;
	m_SnapshotStorage.PurgeAll();
	m_RecivedSnapshots = 0;
	m_SnapshotParts = 0;
	m_PredTick = 0;
	m_CurrentRecvTick = 0;
	m_CurGameTick = 0;
	m_PrevGameTick = 0;
}