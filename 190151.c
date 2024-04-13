int CServer::LoadMap(const char *pMapName)
{
	//DATAFILE *df;
	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "maps/%s.map", pMapName);

	/*df = datafile_load(buf);
	if(!df)
		return 0;*/

	// check for valid standard map
	if(!m_MapChecker.ReadAndValidateMap(Storage(), aBuf, IStorage::TYPE_ALL))
	{
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mapchecker", "invalid standard map");
		return 0;
	}

	if(!m_pMap->Load(aBuf))
		return 0;

	// stop recording when we change map
	m_DemoRecorder.Stop();

	// reinit snapshot ids
	m_IDPool.TimeoutIDs();

	// get the crc of the map
	m_CurrentMapCrc = m_pMap->Crc();
	char aBufMsg[256];
	str_format(aBufMsg, sizeof(aBufMsg), "%s crc is %08x", aBuf, m_CurrentMapCrc);
	Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "server", aBufMsg);

	str_copy(m_aCurrentMap, pMapName, sizeof(m_aCurrentMap));
	//map_set(df);

	// load complete map into memory for download
	{
		IOHANDLE File = Storage()->OpenFile(aBuf, IOFLAG_READ, IStorage::TYPE_ALL);
		m_CurrentMapSize = (int)io_length(File);
		if(m_pCurrentMapData)
			mem_free(m_pCurrentMapData);
		m_pCurrentMapData = (unsigned char *)mem_alloc(m_CurrentMapSize, 1);
		io_read(File, m_pCurrentMapData, m_CurrentMapSize);
		io_close(File);
	}
	return 1;
}