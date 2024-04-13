mp_sint32 PlayerGeneric::getCurrentSample(mp_sint32 position, mp_sint32 channel)
{
	if (mixer)
		return mixer->getCurrentSample(position, channel);
	
	return 0;
}