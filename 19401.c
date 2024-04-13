mp_sint32 PlayerGeneric::stopPlaying()
{
	if (player)
		player->stopPlaying();

	if (mixer)
		return mixer->stop();
		
	return MP_OK;
}