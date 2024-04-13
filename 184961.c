void CClient::RegisterCommands()
{
	m_pConsole = Kernel()->RequestInterface<IConsole>();
	// register server dummy commands for tab completion
	m_pConsole->Register("kick", "i?r", CFGFLAG_SERVER, 0, 0, "Kick player with specified id for any reason");
	m_pConsole->Register("ban", "s?ir", CFGFLAG_SERVER, 0, 0, "Ban player with ip/id for x minutes for any reason");
	m_pConsole->Register("unban", "s", CFGFLAG_SERVER, 0, 0, "Unban ip");
	m_pConsole->Register("bans", "", CFGFLAG_SERVER, 0, 0, "Show banlist");
	m_pConsole->Register("status", "", CFGFLAG_SERVER, 0, 0, "List players");
	m_pConsole->Register("shutdown", "", CFGFLAG_SERVER, 0, 0, "Shut down");
	m_pConsole->Register("record", "?s", CFGFLAG_SERVER, 0, 0, "Record to a file");
	m_pConsole->Register("stoprecord", "", CFGFLAG_SERVER, 0, 0, "Stop recording");
	m_pConsole->Register("reload", "", CFGFLAG_SERVER, 0, 0, "Reload the map");

	m_pConsole->Register("quit", "", CFGFLAG_CLIENT|CFGFLAG_STORE, Con_Quit, this, "Quit Teeworlds");
	m_pConsole->Register("exit", "", CFGFLAG_CLIENT|CFGFLAG_STORE, Con_Quit, this, "Quit Teeworlds");
	m_pConsole->Register("minimize", "", CFGFLAG_CLIENT|CFGFLAG_STORE, Con_Minimize, this, "Minimize Teeworlds");
	m_pConsole->Register("connect", "s", CFGFLAG_CLIENT, Con_Connect, this, "Connect to the specified host/ip");
	m_pConsole->Register("disconnect", "", CFGFLAG_CLIENT, Con_Disconnect, this, "Disconnect from the server");
	m_pConsole->Register("ping", "", CFGFLAG_CLIENT, Con_Ping, this, "Ping the current server");
	m_pConsole->Register("screenshot", "", CFGFLAG_CLIENT, Con_Screenshot, this, "Take a screenshot");
	m_pConsole->Register("rcon", "r", CFGFLAG_CLIENT, Con_Rcon, this, "Send specified command to rcon");
	m_pConsole->Register("rcon_auth", "s", CFGFLAG_CLIENT, Con_RconAuth, this, "Authenticate to rcon");
	m_pConsole->Register("play", "r", CFGFLAG_CLIENT|CFGFLAG_STORE, Con_Play, this, "Play the file specified");
	m_pConsole->Register("record", "?s", CFGFLAG_CLIENT, Con_Record, this, "Record to the file");
	m_pConsole->Register("stoprecord", "", CFGFLAG_CLIENT, Con_StopRecord, this, "Stop recording");
	m_pConsole->Register("add_demomarker", "", CFGFLAG_CLIENT, Con_AddDemoMarker, this, "Add demo timeline marker");
	m_pConsole->Register("add_favorite", "s", CFGFLAG_CLIENT, Con_AddFavorite, this, "Add a server as a favorite");
	m_pConsole->Register("remove_favorite", "s", CFGFLAG_CLIENT, Con_RemoveFavorite, this, "Remove a server from favorites");

	// used for server browser update
	m_pConsole->Chain("br_filter_string", ConchainServerBrowserUpdate, this);
	m_pConsole->Chain("br_filter_gametype", ConchainServerBrowserUpdate, this);
	m_pConsole->Chain("br_filter_serveraddress", ConchainServerBrowserUpdate, this);
}