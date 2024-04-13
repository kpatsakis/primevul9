void CClient::Con_AddDemoMarker(IConsole::IResult *pResult, void *pUserData)
{
	CClient *pSelf = (CClient *)pUserData;
	pSelf->DemoRecorder_AddDemoMarker();
}