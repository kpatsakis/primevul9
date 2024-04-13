mp_uint32 PlayerGeneric::getSyncSampleCounter() const
{
	if (player)
		return player->getSyncSampleCounter();
		
	return 0;
}