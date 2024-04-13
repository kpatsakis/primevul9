void CClient::RegisterInterfaces()
{
	Kernel()->RegisterInterface(static_cast<IDemoRecorder*>(&m_DemoRecorder));
	Kernel()->RegisterInterface(static_cast<IDemoPlayer*>(&m_DemoPlayer));
	Kernel()->RegisterInterface(static_cast<IServerBrowser*>(&m_ServerBrowser));
	Kernel()->RegisterInterface(static_cast<IFriends*>(&m_Friends));
}