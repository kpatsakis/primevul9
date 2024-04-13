void CServer::SendServerInfo(const NETADDR *pAddr, int Token)
{
	CNetChunk Packet;
	CPacker p;
	char aBuf[128];

	// count the players
	int PlayerCount = 0, ClientCount = 0;
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(m_aClients[i].m_State != CClient::STATE_EMPTY)
		{
			if(GameServer()->IsClientPlayer(i))
				PlayerCount++;

			ClientCount++;
		}
	}

	p.Reset();

	p.AddRaw(SERVERBROWSE_INFO, sizeof(SERVERBROWSE_INFO));
	str_format(aBuf, sizeof(aBuf), "%d", Token);
	p.AddString(aBuf, 6);

	p.AddString(GameServer()->Version(), 32);
	p.AddString(g_Config.m_SvName, 64);
	p.AddString(GetMapName(), 32);

	// gametype
	p.AddString(GameServer()->GameType(), 16);

	// flags
	int i = 0;
	if(g_Config.m_Password[0]) // password set
		i |= SERVER_FLAG_PASSWORD;
	str_format(aBuf, sizeof(aBuf), "%d", i);
	p.AddString(aBuf, 2);

	str_format(aBuf, sizeof(aBuf), "%d", PlayerCount); p.AddString(aBuf, 3); // num players
	str_format(aBuf, sizeof(aBuf), "%d", m_NetServer.MaxClients()-g_Config.m_SvSpectatorSlots); p.AddString(aBuf, 3); // max players
	str_format(aBuf, sizeof(aBuf), "%d", ClientCount); p.AddString(aBuf, 3); // num clients
	str_format(aBuf, sizeof(aBuf), "%d", m_NetServer.MaxClients()); p.AddString(aBuf, 3); // max clients

	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if(m_aClients[i].m_State != CClient::STATE_EMPTY)
		{
			p.AddString(ClientName(i), MAX_NAME_LENGTH); // client name
			p.AddString(ClientClan(i), MAX_CLAN_LENGTH); // client clan
			str_format(aBuf, sizeof(aBuf), "%d", m_aClients[i].m_Country); p.AddString(aBuf, 6); // client country
			str_format(aBuf, sizeof(aBuf), "%d", m_aClients[i].m_Score); p.AddString(aBuf, 6); // client score
			str_format(aBuf, sizeof(aBuf), "%d", GameServer()->IsClientPlayer(i)?1:0); p.AddString(aBuf, 2); // is player?
		}
	}

	Packet.m_ClientID = -1;
	Packet.m_Address = *pAddr;
	Packet.m_Flags = NETSENDFLAG_CONNLESS;
	Packet.m_DataSize = p.Size();
	Packet.m_pData = p.Data();
	m_NetServer.Send(&Packet);
}