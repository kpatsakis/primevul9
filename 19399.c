mp_sint32 PlayerGeneric::getCurrentSamplePeak(mp_sint32 position, mp_sint32 channel)
{
	if (mixer)
		return mixer->getCurrentSamplePeak(position, channel);

	return 0;
}