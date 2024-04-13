bool PlayerGeneric::getAllowFilters() const
{
	if (player)
		return player->getAllowFilters();
		
	return allowFilters;
}