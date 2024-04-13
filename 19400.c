void PlayerGeneric::setPlayMode(PlayModes mode)
{
	playMode = mode;
	if (player)
		player->setPlayMode(mode);
}