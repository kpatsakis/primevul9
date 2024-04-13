mp_sint32 PlayerGeneric::getRow() const
{
	if (player)
	{
		mp_uint32 index = player->getBeatIndexFromSamplePos(getCurrentSamplePosition());
		return player->getRow(index);
	}
	
	return 0;
}