void PlayerGeneric::setDisableMixing(bool b)
{
	disableMixing = b;

	if (player)
		player->setDisableMixing(disableMixing);
}