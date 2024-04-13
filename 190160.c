void CServer::PumpNetwork()
{
	CNetChunk Packet;

	m_NetServer.Update();

	// process packets
	while(m_NetServer.Recv(&Packet))
	{
		if(Packet.m_ClientID == -1)
		{
			// stateless
			if(!m_Register.RegisterProcessPacket(&Packet))
			{
				if(Packet.m_DataSize == sizeof(SERVERBROWSE_GETINFO)+1 &&
					mem_comp(Packet.m_pData, SERVERBROWSE_GETINFO, sizeof(SERVERBROWSE_GETINFO)) == 0)
				{
					SendServerInfo(&Packet.m_Address, ((unsigned char *)Packet.m_pData)[sizeof(SERVERBROWSE_GETINFO)]);
				}
			}
		}
		else
			ProcessClientPacket(&Packet);
	}

	m_ServerBan.Update();
	m_Econ.Update();
}