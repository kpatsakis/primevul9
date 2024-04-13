void CClient::Con_Play(IConsole::IResult *pResult, void *pUserData)
{
	CClient *pSelf = (CClient *)pUserData;
	pSelf->DemoPlayer_Play(pResult->GetString(0), IStorage::TYPE_ALL);
}