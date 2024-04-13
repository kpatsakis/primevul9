void PlayerGeneric::setResamplerType(bool interpolation, bool ramping)
{
	if (interpolation)
	{
		if (ramping)
			resamplerType = MIXER_LERPING_RAMPING;
		else
			resamplerType = MIXER_LERPING;
	}
	else
	{
		if (ramping)
			resamplerType = MIXER_NORMAL_RAMPING;
		else
			resamplerType = MIXER_NORMAL;
	}

	if (player)
		player->setResamplerType(resamplerType);
}