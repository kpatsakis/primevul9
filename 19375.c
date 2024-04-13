bool PlayerGeneric::isIdle() const
{
	if (player)
		return player->isIdle();
		
	return idle;
}