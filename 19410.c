mp_sint32 PlayerGeneric::startPlaying(XModule* module, 
									  bool repeat/* = false*/, 
									  mp_uint32 startPosition/* = 0*/, 
									  mp_uint32 startRow/* = 0*/,
									  mp_sint32 numChannels/* = -1*/, 
									  const mp_ubyte* customPanningTable/* = NULL*/,
									  bool idle/* = false*/,
									  mp_sint32 patternIndex/* = -1*/,
									  bool playOneRowOnly/* = false*/)
{
	this->idle = idle;
	this->repeat = repeat;
	this->playOneRowOnly = playOneRowOnly;

	if (mixer == NULL)
	{
		mixer = new MasterMixer(frequency, bufferSize, 1, audioDriver);
		mixer->setMasterMixerNotificationListener(listener);
		mixer->setSampleShift(sampleShift);
		if (audioDriver == NULL)
			mixer->setCurrentAudioDriverByName(audioDriverName);
	}

	if (!player || player->getType() != getPreferredPlayerType(module))
	{
		if (player)
		{
			if (!mixer->isDeviceRemoved(player))
				mixer->removeDevice(player);
			delete player;
		}
		
		player = getPreferredPlayer(module);
		
		if (player)
		{
			// apply our own "state" to the state of the newly allocated player
			player->resetMainVolumeOnStartPlay(resetMainVolumeOnStartPlayFlag);
			player->resetOnStop(resetOnStopFlag);
			player->setBufferSize(bufferSize);
			player->setResamplerType(resamplerType);
			player->setMasterVolume(masterVolume);
			player->setPanningSeparation(panningSeparation);
			player->setPlayMode(playMode);

			for (mp_sint32 i = PlayModeOptionFirst; i < PlayModeOptionLast; i++)
				player->enable((PlayModeOptions)i, options[i]);			
			
			player->setDisableMixing(disableMixing);
			player->setAllowFilters(allowFilters);
			//if (paused)
			//	player->pausePlaying();

			// adjust number of virtual channels if necessary
			setNumMaxVirChannels(numMaxVirChannels);
		}		
	}
	
	if (player && mixer)
	{
		if (!mixer->isDeviceRemoved(player))
			mixer->removeDevice(player);
			
		player->startPlaying(module, repeat, startPosition, startRow, numChannels, customPanningTable, idle, patternIndex, playOneRowOnly);
		
		mixer->addDevice(player);
		
		if (!mixer->isPlaying())
			return mixer->start();
	}


	return MP_OK;
}