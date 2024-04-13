int CServer::TrySetClientName(int ClientID, const char *pName)
{
	char aTrimmedName[64];

	// trim the name
	str_copy(aTrimmedName, StrLtrim(pName), sizeof(aTrimmedName));
	StrRtrim(aTrimmedName);

	// check for empty names
	if(!aTrimmedName[0])
		return -1;

	// check if new and old name are the same
	if(m_aClients[ClientID].m_aName[0] && str_comp(m_aClients[ClientID].m_aName, aTrimmedName) == 0)
		return 0;

	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "'%s' -> '%s'", pName, aTrimmedName);
	Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "server", aBuf);
	pName = aTrimmedName;

	// make sure that two clients doesn't have the same name
	for(int i = 0; i < MAX_CLIENTS; i++)
		if(i != ClientID && m_aClients[i].m_State >= CClient::STATE_READY)
		{
			if(str_comp(pName, m_aClients[i].m_aName) == 0)
				return -1;
		}

	// set the client name
	str_copy(m_aClients[ClientID].m_aName, pName, MAX_NAME_LENGTH);
	return 0;
}