mp_sint32 PlayerGeneric::getNumMaxVirChannels() const
{
#ifndef MILKYTRACKER
	if (player)
	{
		if (player->getType() == PlayerBase::PlayerType_IT)
		{
			return static_cast<PlayerIT*>(player)->getNumMaxVirChannels();
		}
	}
#endif
	return numMaxVirChannels;
}