void CClient::Con_Minimize(IConsole::IResult *pResult, void *pUserData)
{
	CClient *pSelf = (CClient *)pUserData;
	pSelf->Graphics()->Minimize();
}