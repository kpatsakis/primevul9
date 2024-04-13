mp_sint32 PlayerGeneric::getMasterVolume() const
{
	if (player)
		return player->getMasterVolume();
		
	return masterVolume;
}