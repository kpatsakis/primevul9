void PlayerGeneric::setMasterVolume(mp_sint32 vol)
{
	masterVolume = vol;
	if (player)
		player->setMasterVolume(vol);
}