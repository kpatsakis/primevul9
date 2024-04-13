bool PlayerGeneric::hasSongHalted() const
{
	if (player)
		return player->hasSongHalted();
		
	return true;
}