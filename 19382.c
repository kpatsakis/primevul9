mp_sint32 PlayerGeneric::getPanningSeparation() const
{
	if (player)
		return player->getPanningSeparation();
		
	return panningSeparation;
}