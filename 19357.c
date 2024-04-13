mp_sint32 PlayerGeneric::pausePlaying()
{
	paused = true;
	if (mixer)
		return mixer->pause();
		
	return MP_OK;
}