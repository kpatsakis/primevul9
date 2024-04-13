bool PlayerGeneric::isRepeating() const
{
	if (player)
		return player->isRepeating();
		
	return repeat;
}