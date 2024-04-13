void CClient::Con_RconAuth(IConsole::IResult *pResult, void *pUserData)
{
	CClient *pSelf = (CClient *)pUserData;
	pSelf->RconAuth("", pResult->GetString(0));
}