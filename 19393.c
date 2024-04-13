mp_sint32 PlayerGeneric::getCurrentSamplePosition() const
{
	if (mixer && mixer->getAudioDriver())
		return mixer->getAudioDriver()->getBufferPos();
	
	return 0;
}