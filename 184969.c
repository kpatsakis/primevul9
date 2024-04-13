void CClient::Con_Record(IConsole::IResult *pResult, void *pUserData)
{
	CClient *pSelf = (CClient *)pUserData;
	if(pResult->NumArguments())
		pSelf->DemoRecorder_Start(pResult->GetString(0), false);
	else
		pSelf->DemoRecorder_Start("demo", true);
}