static void wcd9335_codec_init(struct snd_soc_component *component)
{
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);
	int i;

	/* ungate MCLK and set clk rate */
	regmap_update_bits(wcd->regmap, WCD9335_CODEC_RPM_CLK_GATE,
				WCD9335_CODEC_RPM_CLK_GATE_MCLK_GATE_MASK, 0);

	regmap_update_bits(wcd->regmap, WCD9335_CODEC_RPM_CLK_MCLK_CFG,
				WCD9335_CODEC_RPM_CLK_MCLK_CFG_MCLK_MASK,
				WCD9335_CODEC_RPM_CLK_MCLK_CFG_9P6MHZ);

	for (i = 0; i < ARRAY_SIZE(wcd9335_codec_reg_init); i++)
		snd_soc_component_update_bits(component,
					wcd9335_codec_reg_init[i].reg,
					wcd9335_codec_reg_init[i].mask,
					wcd9335_codec_reg_init[i].val);

	wcd9335_enable_efuse_sensing(component);
}