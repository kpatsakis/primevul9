void PlayerGeneric::setRepeat(bool repeat)
{
	this->repeat = repeat;
	if (player)
		player->setRepeat(repeat);
}