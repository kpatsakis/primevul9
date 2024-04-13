mp_sint32 PlayerGeneric::getLastUnvisitedPosition() const
{
	if (player)
		return player->getLastUnvisitedPosition();
	
	return 0;
}