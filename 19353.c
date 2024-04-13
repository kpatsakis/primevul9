void PlayerGeneric::restart(mp_uint32 startPosition/* = 0*/, mp_uint32 startRow/* = 0*/, bool resetMixer/* = true*/, const mp_ubyte* customPanningTable/* = NULL*/, bool playOneRowOnly/* = false*/)
{
	if (player)
		player->restart(startPosition, startRow, resetMixer, customPanningTable, playOneRowOnly);
}