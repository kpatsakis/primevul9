ChannelMixer::ResamplerTypes PlayerGeneric::getResamplerType() const
{
	if (player)
		return player->getResamplerType();
		
	return resamplerType;
}