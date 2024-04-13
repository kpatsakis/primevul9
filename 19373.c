void PlayerGeneric::setNumMaxVirChannels(mp_sint32 max)
{
	numMaxVirChannels = max;
#ifndef MILKYTRACKER
	if (player)
	{
		if (player->getType() == PlayerBase::PlayerType_IT)
		{
			static_cast<PlayerIT*>(player)->setNumMaxVirChannels(max);
		}
	}
#endif
}