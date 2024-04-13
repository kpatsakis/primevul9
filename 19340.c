void PlayerGeneric::setResamplerType(ResamplerTypes type)
{
	resamplerType = type;
	if (player)
		player->setResamplerType(type);
}