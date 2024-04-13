mp_sint32 PlayerGeneric::getOrder() const
{
	if (player)
	{
		mp_uint32 index = player->getBeatIndexFromSamplePos(getCurrentSamplePosition());
		return player->getOrder(index);
	}
		
	return 0;
}