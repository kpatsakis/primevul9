void PlayerGeneric::setPanning(mp_ubyte chn, mp_ubyte pan)
{
	if (player)
		player->setPanning(chn, pan);
}