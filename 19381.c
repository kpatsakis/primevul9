mp_sint32 PlayerGeneric::adjustFrequency(mp_uint32 frequency)
{
	this->frequency = frequency;
	
	mp_sint32 res = MP_OK;
	
	if (mixer)
		res = mixer->setSampleRate(frequency);
	
	return res;
}