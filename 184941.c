void CClient::Con_Quit(IConsole::IResult *pResult, void *pUserData)
{
	CClient *pSelf = (CClient *)pUserData;
	pSelf->Quit();
}