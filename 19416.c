void PlayerGeneric::setPatternPos(mp_uint32 pos, mp_uint32 row/* = 0*/, bool resetChannels/* = true*/, bool resetFXMemory/* = true*/)
{
	if (player)
		player->setPatternPos(pos, row, resetChannels, resetFXMemory);
}