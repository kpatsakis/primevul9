void CClient::SnapInvalidateItem(int SnapID, int Index)
{
	CSnapshotItem *i;
	dbg_assert(SnapID >= 0 && SnapID < NUM_SNAPSHOT_TYPES, "invalid SnapID");
	i = m_aSnapshots[SnapID]->m_pAltSnap->GetItem(Index);
	if(i)
	{
		if((char *)i < (char *)m_aSnapshots[SnapID]->m_pAltSnap || (char *)i > (char *)m_aSnapshots[SnapID]->m_pAltSnap + m_aSnapshots[SnapID]->m_SnapSize)
			m_pConsole->Print(IConsole::OUTPUT_LEVEL_DEBUG, "client", "snap invalidate problem");
		if((char *)i >= (char *)m_aSnapshots[SnapID]->m_pSnap && (char *)i < (char *)m_aSnapshots[SnapID]->m_pSnap + m_aSnapshots[SnapID]->m_SnapSize)
			m_pConsole->Print(IConsole::OUTPUT_LEVEL_DEBUG, "client", "snap invalidate problem");
		i->m_TypeAndID = -1;
	}
}