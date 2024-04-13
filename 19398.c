const char*	PlayerGeneric::getCurrentAudioDriverName() const
{
	if (mixer)
		return mixer->getCurrentAudioDriverName();

	return audioDriverName;
}