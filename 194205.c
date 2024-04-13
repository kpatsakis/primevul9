void MsgSetRawMsgWOSize(msg_t *pMsg, char* pszRawMsg)
{
	MsgSetRawMsg(pMsg, pszRawMsg, strlen(pszRawMsg));
}