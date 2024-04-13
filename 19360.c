void PlayerGeneric::setIdle(bool idle)
{
	this->idle = idle;
	if (player)
		player->setIdle(idle);
}