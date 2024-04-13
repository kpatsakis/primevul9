mp_int64 PlayerGeneric::getSyncCount() const
{
	if (player)
		return player->getSyncCount();
		
	return 0;
}