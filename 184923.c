void CClient::VersionUpdate()
{
	if(m_VersionInfo.m_State == CVersionInfo::STATE_INIT)
	{
		Engine()->HostLookup(&m_VersionInfo.m_VersionServeraddr, g_Config.m_ClVersionServer, m_ContactClient.NetType());
		m_VersionInfo.m_State = CVersionInfo::STATE_START;
	}
	else if(m_VersionInfo.m_State == CVersionInfo::STATE_START)
	{
		if(m_VersionInfo.m_VersionServeraddr.m_Job.Status() == CJob::STATE_DONE)
		{
			CNetChunk Packet;

			mem_zero(&Packet, sizeof(Packet));

			m_VersionInfo.m_VersionServeraddr.m_Addr.port = VERSIONSRV_PORT;

			Packet.m_ClientID = -1;
			Packet.m_Address = m_VersionInfo.m_VersionServeraddr.m_Addr;
			Packet.m_pData = VERSIONSRV_GETVERSION;
			Packet.m_DataSize = sizeof(VERSIONSRV_GETVERSION);
			Packet.m_Flags = NETSENDFLAG_CONNLESS;

			m_ContactClient.Send(&Packet);
			m_VersionInfo.m_State = CVersionInfo::STATE_READY;
		}
	}
}