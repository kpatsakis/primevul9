void CClient::Con_Disconnect(IConsole::IResult *pResult, void *pUserData)
{
	CClient *pSelf = (CClient *)pUserData;
	pSelf->Disconnect();
}