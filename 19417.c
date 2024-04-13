void PlayerGeneric::getPosition(mp_sint32& order, mp_sint32& row, mp_sint32& ticker) const
{
	if (player)
	{
		mp_uint32 index = player->getBeatIndexFromSamplePos(getCurrentSamplePosition());
		player->getPosition(order, row, ticker, index);
		return;
	}
	
	order = row = ticker = 0;
}