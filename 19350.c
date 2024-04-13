void PlayerGeneric::enable(PlayModeOptions option, bool b)
{
	ASSERT(option>=PlayModeOptionFirst && option<PlayModeOptionLast);
	options[option] = b;
	
	if (player)
		player->enable(option, b);
}