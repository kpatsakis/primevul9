bool PlayerGeneric::isEnabled(PlayModeOptions option) const
{
	ASSERT(option>=PlayModeOptionFirst && option<PlayModeOptionLast);
	
	if (!player)
		return options[option];
	else 
		return player->isEnabled(option);
}