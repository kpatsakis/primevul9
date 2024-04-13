mp_sint32 PlayerGeneric::getCurrentBeatIndex()
{
	if (player)
		return player->getBeatIndexFromSamplePos(getCurrentSamplePosition());
	
	return 0;
}