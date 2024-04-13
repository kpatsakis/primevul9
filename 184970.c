void CClient::SendReady()
{
	CMsgPacker Msg(NETMSG_READY);
	SendMsgEx(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH);
}