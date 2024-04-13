PlayerBase*	PlayerGeneric::getPreferredPlayer(XModule* module) const
{
	switch (getPreferredPlayerType(module))
	{
#ifndef MILKYTRACKER
		case PlayerBase::PlayerType_FAR:
			return new PlayerFAR(frequency);
		case PlayerBase::PlayerType_IT:
			return new PlayerIT(frequency);
#endif
		case PlayerBase::PlayerType_Generic:
			return new PlayerSTD(frequency);
			
		default:
			return NULL;
	}
}