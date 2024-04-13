mp_sint32 PlayerGeneric::exportToWAV(const SYSCHAR* fileName, XModule* module, 
									 mp_sint32 startOrder/* = 0*/, mp_sint32 endOrder/* = -1*/, 
									 const mp_ubyte* mutingArray/* = NULL*/, mp_uint32 mutingNumChannels/* = 0*/,
									 const mp_ubyte* customPanningTable/* = NULL*/,
									 AudioDriverBase* preferredDriver/* = NULL*/,
									 mp_sint32* timingLUT/* = NULL*/)
{
	PlayerBase* player = NULL;
	
	AudioDriverBase* wavWriter = preferredDriver;
	bool isWAVWriterDriver = false;
	
	if (wavWriter == NULL)
	{
		wavWriter = new WAVWriter(fileName);
		isWAVWriterDriver = true;
	
		if (!static_cast<WAVWriter*>(wavWriter)->isOpen())
		{
			delete wavWriter;
			return MP_DEVICE_ERROR;
		}
	}
	
	MasterMixer mixer(frequency, bufferSize, 1, wavWriter);
	mixer.setSampleShift(sampleShift);
	mixer.setDisableMixing(disableMixing);
	
	player = getPreferredPlayer(module);
	
	PeakAutoAdjustFilter filter;
	if (autoAdjustPeak)
		mixer.setFilterHook(&filter);
		
	if (player)
	{
		player->adjustFrequency(frequency);
		player->resetOnStop(resetOnStopFlag);
		player->setBufferSize(bufferSize);
		player->setResamplerType(resamplerType);
		player->setMasterVolume(masterVolume);
		player->setPlayMode(playMode);
		player->setDisableMixing(disableMixing);
		player->setAllowFilters(allowFilters);		
#ifndef MILKYTRACKER
		if (player->getType() == PlayerBase::PlayerType_IT)
		{
			static_cast<PlayerIT*>(player)->setNumMaxVirChannels(numMaxVirChannels);
		}
#endif
		mixer.addDevice(player);
	}

	if (player)
	{
		if (mutingArray && mutingNumChannels > 0 && mutingNumChannels <= module->header.channum)
		{
			for (mp_uint32 i = 0; i < mutingNumChannels; i++)
				player->muteChannel(i, mutingArray[i] == 1);
		}
		player->startPlaying(module, false, startOrder, 0, -1, customPanningTable, false, -1);
		
		mixer.start();
	}

	if (endOrder == -1 || endOrder < startOrder || endOrder > module->header.ordnum - 1)
		endOrder = module->header.ordnum - 1;		

	mp_sint32 curOrderPos = startOrder;
	if (timingLUT)
	{
		for (mp_sint32 i = 0; i < module->header.ordnum; i++)
			timingLUT[i] = -1;
		
		timingLUT[curOrderPos] = 0;
	}

	while (!player->hasSongHalted() && player->getOrder(0) <= endOrder)
	{
		wavWriter->advance();

		if (player->getOrder(0) != curOrderPos)
		{
#ifdef __VERBOSE__
			printf("%f\n", (float)wavWriter->getNumPlayedSamples() / (float)getMixFrequency());
#endif
			curOrderPos = player->getOrder(0);
			if (timingLUT && curOrderPos < module->header.ordnum && timingLUT[curOrderPos] == -1)
				timingLUT[curOrderPos] = wavWriter->getNumPlayedSamples();			
		}
	}

	player->stopPlaying();
	
	mixer.stop();
	// important step, otherwise destruction of the audio driver will cause
	// trouble if the mixer instance is removed from this function's stack 
	// and trys to access the driver which is no longer existant
	mixer.closeAudioDevice();

	// Sync value
	sampleShift = mixer.getSampleShift();
	filter.mixerShift = sampleShift;
	filter.calculateMasterVolume();
	masterVolume = filter.masterVolume;

	delete player;

	mp_sint32 numWrittenSamples = wavWriter->getNumPlayedSamples();
	
	if (isWAVWriterDriver)
		delete wavWriter;

	return numWrittenSamples;
}