void PlayerGeneric::setSampleShift(mp_sint32 shift)
{
	sampleShift = shift;
	if (mixer)
		mixer->setSampleShift(shift);
}