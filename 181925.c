static int wcd9335_enable_efuse_sensing(struct snd_soc_component *comp)
{
	_wcd9335_codec_enable_mclk(comp, true);
	snd_soc_component_update_bits(comp,
				WCD9335_CHIP_TIER_CTRL_EFUSE_CTL,
				WCD9335_CHIP_TIER_CTRL_EFUSE_EN_MASK,
				WCD9335_CHIP_TIER_CTRL_EFUSE_ENABLE);
	/*
	 * 5ms sleep required after enabling efuse control
	 * before checking the status.
	 */
	usleep_range(5000, 5500);

	if (!(snd_soc_component_read32(comp,
					WCD9335_CHIP_TIER_CTRL_EFUSE_STATUS) &
					WCD9335_CHIP_TIER_CTRL_EFUSE_EN_MASK))
		WARN(1, "%s: Efuse sense is not complete\n", __func__);

	wcd9335_enable_sido_buck(comp);
	_wcd9335_codec_enable_mclk(comp, false);

	return 0;
}