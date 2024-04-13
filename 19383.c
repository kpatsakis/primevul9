void PlayerGeneric::setPanningSeparation(mp_sint32 separation)
{
	panningSeparation = separation;
	if (player)
		player->setPanningSeparation(separation);
}