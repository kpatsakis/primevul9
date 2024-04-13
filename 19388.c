mp_sint32 PlayerGeneric::getSampleShift() const
{
	if (mixer)
		return mixer->getSampleShift();
	
	return sampleShift;
}