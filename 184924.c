void CClient::SendInfo()
{
	CMsgPacker Msg(NETMSG_INFO);
	Msg.AddString(GameClient()->NetVersion(), 128);
	Msg.AddString(g_Config.m_Password, 128);
	SendMsgEx(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH);
}