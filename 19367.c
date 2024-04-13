mp_sint32 PlayerGeneric::getNumActiveChannels() const
{
	if (player)
		return player->getNumActiveChannels();

	return 0;
}