PlayerGeneric::PlayerGeneric(mp_sint32 frequency, AudioDriverInterface* audioDriver/* = NULL*/) :
	mixer(NULL),
	player(NULL),
	frequency(frequency),
	audioDriver(audioDriver),
	audioDriverName(NULL)
{
	listener = new MixerNotificationListener(*this);

	bufferSize = 0;
	sampleShift = 0;
	
	resamplerType = MIXER_NORMAL;

	idle = false;
	playOneRowOnly = false;
	paused = false;
	repeat = false;
	resetOnStopFlag = false;
	autoAdjustPeak = false;
	disableMixing = false;
	allowFilters = false;
#ifdef __FORCEPOWEROFTWOBUFFERSIZE__
	compensateBufferFlag = true;
#else
	compensateBufferFlag = false;
#endif
	masterVolume = panningSeparation = numMaxVirChannels = 256;
	resetMainVolumeOnStartPlayFlag = true;
	playMode = PlayMode_Auto;

	// Special playmode settings
	options[PlayModeOptionPanning8xx] = true;
	options[PlayModeOptionPanningE8x] = false;
	options[PlayModeOptionForcePTPitchLimit] = true;

	AudioDriverManager audioDriverManager;
	const char* defaultName = audioDriverManager.getPreferredAudioDriver()->getDriverID();
	if (defaultName)
	{
		audioDriverName = new char[strlen(defaultName)+1];
		strcpy(audioDriverName, defaultName);
	}
}