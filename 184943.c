void CClient::Con_AddFavorite(IConsole::IResult *pResult, void *pUserData)
{
	CClient *pSelf = (CClient *)pUserData;
	NETADDR Addr;
	if(net_addr_from_str(&Addr, pResult->GetString(0)) == 0)
		pSelf->m_ServerBrowser.AddFavorite(Addr);
}