void PlayerGeneric::resetMainVolumeOnStartPlay(bool b)
{
	resetMainVolumeOnStartPlayFlag = b;
	if (player)
		player->resetMainVolumeOnStartPlay(b); 
}