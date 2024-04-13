mp_sint32 PlayerGeneric::getNumAllocatedChannels() const
{
	if (player)
		return player->getNumAllocatedChannels();

	return 0;
}