void CClient::PumpNetwork()
{
	m_NetClient.Update();

	if(State() != IClient::STATE_DEMOPLAYBACK)
	{
		// check for errors
		if(State() != IClient::STATE_OFFLINE && State() != IClient::STATE_QUITING && m_NetClient.State() == NETSTATE_OFFLINE)
		{
			SetState(IClient::STATE_OFFLINE);
			Disconnect();
			char aBuf[256];
			str_format(aBuf, sizeof(aBuf), "offline error='%s'", m_NetClient.ErrorString());
			m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "client", aBuf);
		}

		//
		if(State() == IClient::STATE_CONNECTING && m_NetClient.State() == NETSTATE_ONLINE)
		{
			// we switched to online
			m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "client", "connected, sending info");
			SetState(IClient::STATE_LOADING);
			SendInfo();
		}
	}

	// process non-connless packets
	CNetChunk Packet;
	while(m_NetClient.Recv(&Packet))
	{
		if(Packet.m_ClientID != -1)
			ProcessServerPacket(&Packet);
	}

	// process connless packets data
	m_ContactClient.Update();
	while(m_ContactClient.Recv(&Packet))
	{
		if(Packet.m_ClientID == -1)
			ProcessConnlessPacket(&Packet);
	}
}