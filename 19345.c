PlayerGeneric::PlayModes PlayerGeneric::getPlayMode() const
{
	if (player)
		return player->getPlayMode();
		
	return playMode;
}