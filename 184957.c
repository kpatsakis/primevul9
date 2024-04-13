void CClient::Con_Connect(IConsole::IResult *pResult, void *pUserData)
{
	CClient *pSelf = (CClient *)pUserData;
	str_copy(pSelf->m_aCmdConnect, pResult->GetString(0), sizeof(pSelf->m_aCmdConnect));
}