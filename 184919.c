const char *CClient::LoadMapSearch(const char *pMapName, int WantedCrc)
{
	const char *pError = 0;
	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "loading map, map=%s wanted crc=%08x", pMapName, WantedCrc);
	m_pConsole->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "client", aBuf);
	SetState(IClient::STATE_LOADING);

	// try the normal maps folder
	str_format(aBuf, sizeof(aBuf), "maps/%s.map", pMapName);
	pError = LoadMap(pMapName, aBuf, WantedCrc);
	if(!pError)
		return pError;

	// try the downloaded maps
	str_format(aBuf, sizeof(aBuf), "downloadedmaps/%s_%08x.map", pMapName, WantedCrc);
	pError = LoadMap(pMapName, aBuf, WantedCrc);
	if(!pError)
		return pError;

	// search for the map within subfolders
	char aFilename[128];
	str_format(aFilename, sizeof(aFilename), "%s.map", pMapName);
	if(Storage()->FindFile(aFilename, "maps", IStorage::TYPE_ALL, aBuf, sizeof(aBuf)))
		pError = LoadMap(pMapName, aBuf, WantedCrc);

	return pError;
}