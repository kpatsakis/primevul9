mp_sint32 PlayerGeneric::getMixFrequency() const
{
	if (player)
		return player->getMixFrequency();
		
	return frequency;
}