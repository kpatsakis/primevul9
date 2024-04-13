void PlayerGeneric::setAllowFilters(bool b)
{
	allowFilters = b;

	if (player)
		player->setAllowFilters(allowFilters);
}