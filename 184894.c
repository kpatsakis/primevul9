int CClient::SendMsg(CMsgPacker *pMsg, int Flags)
{
	return SendMsgEx(pMsg, Flags, false);
}