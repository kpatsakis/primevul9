int CClient::SendMsgEx(CMsgPacker *pMsg, int Flags, bool System)
{
	CNetChunk Packet;

	if(State() == IClient::STATE_OFFLINE)
		return 0;

	mem_zero(&Packet, sizeof(CNetChunk));

	Packet.m_ClientID = 0;
	Packet.m_pData = pMsg->Data();
	Packet.m_DataSize = pMsg->Size();

	// HACK: modify the message id in the packet and store the system flag
	if(*((unsigned char*)Packet.m_pData) == 1 && System && Packet.m_DataSize == 1)
		dbg_break();

	*((unsigned char*)Packet.m_pData) <<= 1;
	if(System)
		*((unsigned char*)Packet.m_pData) |= 1;

	if(Flags&MSGFLAG_VITAL)
		Packet.m_Flags |= NETSENDFLAG_VITAL;
	if(Flags&MSGFLAG_FLUSH)
		Packet.m_Flags |= NETSENDFLAG_FLUSH;

	if(Flags&MSGFLAG_RECORD)
	{
		if(m_DemoRecorder.IsRecording())
			m_DemoRecorder.RecordMessage(Packet.m_pData, Packet.m_DataSize);
	}

	if(!(Flags&MSGFLAG_NOSEND))
		m_NetClient.Send(&Packet);
	return 0;
}