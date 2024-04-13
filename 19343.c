mp_sint32 PlayerGeneric::getSongMainVolume() const
{
	if (player)
	{
		mp_uint32 index = player->getBeatIndexFromSamplePos(getCurrentSamplePosition());
		return player->getSongMainVolume(index);
	}
	
	return 255;
}