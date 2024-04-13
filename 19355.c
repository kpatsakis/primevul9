void PlayerGeneric::getPosition(mp_sint32& order, mp_sint32& row) const
{
	if (player)
	{
		mp_uint32 index = player->getBeatIndexFromSamplePos(getCurrentSamplePosition());	
		player->getPosition(order, row, index);
		return;
	}
	
	order = row = 0;
}