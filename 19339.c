mp_sint32 PlayerGeneric::resumePlaying()
{
	if (player && !player->isPlaying())
		player->resumePlaying();

	if (mixer && mixer->isPaused())
		return mixer->resume();
	else if (mixer && !mixer->isPlaying())
		return mixer->start();

	return MP_OK;
}