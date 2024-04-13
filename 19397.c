bool PlayerGeneric::setCurrentAudioDriverByName(const char* name)
{
	if (name == NULL)
		return false;

	if (mixer)
	{
		bool res = mixer->setCurrentAudioDriverByName(name);

		if (audioDriverName)
			delete[] audioDriverName;

		const char* curDrvName = getCurrentAudioDriverName();
		ASSERT(curDrvName);
		audioDriverName = new char[strlen(curDrvName)+1];
		strcpy(audioDriverName, curDrvName);
		return res;
	}

	AudioDriverManager audioDriverManager;
	if (audioDriverManager.getAudioDriverByName(name))
	{
		if (audioDriverName)
			delete[] audioDriverName;

		audioDriverName = new char[strlen(name)+1];
		strcpy(audioDriverName, name);		
		return true;
	}

	return false;
}