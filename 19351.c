mp_sint32 PlayerGeneric::getSpeed() const
{
	if (player)
	{
		mp_uint32 index = player->getBeatIndexFromSamplePos(getCurrentSamplePosition());
		return player->getSpeed(index);
	}
		
	return 0;
}