void CServer::ConStatus(IConsole::IResult *pResult, void *pUser)
{
	char aBuf[1024];
	char aAddrStr[NETADDR_MAXSTRSIZE];
	CServer* pThis = static_cast<CServer *>(pUser);

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(pThis->m_aClients[i].m_State != CClient::STATE_EMPTY)
		{
			net_addr_str(pThis->m_NetServer.ClientAddr(i), aAddrStr, sizeof(aAddrStr), true);
			if(pThis->m_aClients[i].m_State == CClient::STATE_INGAME)
			{
				const char *pAuthStr = pThis->m_aClients[i].m_Authed == CServer::AUTHED_ADMIN ? "(Admin)" :
										pThis->m_aClients[i].m_Authed == CServer::AUTHED_MOD ? "(Mod)" : "";
				str_format(aBuf, sizeof(aBuf), "id=%d addr=%s name='%s' score=%d %s", i, aAddrStr,
					pThis->m_aClients[i].m_aName, pThis->m_aClients[i].m_Score, pAuthStr);
			}
			else
				str_format(aBuf, sizeof(aBuf), "id=%d addr=%s connecting", i, aAddrStr);
			pThis->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Server", aBuf);
		}
	}
}