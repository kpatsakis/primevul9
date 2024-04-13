static void wcd9335_enable_sido_buck(struct snd_soc_component *component)
{
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);

	snd_soc_component_update_bits(component, WCD9335_ANA_RCO,
					WCD9335_ANA_RCO_BG_EN_MASK,
					WCD9335_ANA_RCO_BG_ENABLE);
	snd_soc_component_update_bits(component, WCD9335_ANA_BUCK_CTL,
					WCD9335_ANA_BUCK_CTL_VOUT_D_IREF_MASK,
					WCD9335_ANA_BUCK_CTL_VOUT_D_IREF_EXT);
	/* 100us sleep needed after IREF settings */
	usleep_range(100, 110);
	snd_soc_component_update_bits(component, WCD9335_ANA_BUCK_CTL,
					WCD9335_ANA_BUCK_CTL_VOUT_D_VREF_MASK,
					WCD9335_ANA_BUCK_CTL_VOUT_D_VREF_EXT);
	/* 100us sleep needed after VREF settings */
	usleep_range(100, 110);
	wcd->sido_input_src = SIDO_SOURCE_RCO_BG;
}