mp_sint32 PlayerGeneric::getTempo() const
{
	if (player)
	{
		mp_uint32 index = player->getBeatIndexFromSamplePos(getCurrentSamplePosition());
		return player->getTempo(index);
	}
		
	return 0;
}