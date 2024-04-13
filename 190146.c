void CServerBan::InitServerBan(IConsole *pConsole, IStorage *pStorage, CServer* pServer)
{
	CNetBan::Init(pConsole, pStorage);

	m_pServer = pServer;

	// overwrites base command, todo: improve this
	Console()->Register("ban", "s?ir", CFGFLAG_SERVER|CFGFLAG_STORE, ConBanExt, this, "Ban player with ip/client id for x minutes for any reason");
}