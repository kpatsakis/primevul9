void PlayerGeneric::resetOnStop(bool b) 
{
	resetOnStopFlag = b;
	if (player)
		player->resetOnStop(b); 
}