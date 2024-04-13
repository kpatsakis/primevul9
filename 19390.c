mp_int64 PlayerGeneric::getSampleCounter() const
{
	if (player)
		return player->getSampleCounter();
	
	return 0;
}