static int wcd9335_config_compander(struct snd_soc_component *component,
				    int interp_n, int event)
{
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);
	int comp;
	u16 comp_ctl0_reg, rx_path_cfg0_reg;

	/* EAR does not have compander */
	if (!interp_n)
		return 0;

	comp = interp_n - 1;
	if (!wcd->comp_enabled[comp])
		return 0;

	comp_ctl0_reg = WCD9335_CDC_COMPANDER1_CTL(comp);
	rx_path_cfg0_reg = WCD9335_CDC_RX1_RX_PATH_CFG(comp);

	if (SND_SOC_DAPM_EVENT_ON(event)) {
		/* Enable Compander Clock */
		snd_soc_component_update_bits(component, comp_ctl0_reg,
					WCD9335_CDC_COMPANDER_CLK_EN_MASK,
					WCD9335_CDC_COMPANDER_CLK_ENABLE);
		/* Reset comander */
		snd_soc_component_update_bits(component, comp_ctl0_reg,
					WCD9335_CDC_COMPANDER_SOFT_RST_MASK,
					WCD9335_CDC_COMPANDER_SOFT_RST_ENABLE);
		snd_soc_component_update_bits(component, comp_ctl0_reg,
				WCD9335_CDC_COMPANDER_SOFT_RST_MASK,
				WCD9335_CDC_COMPANDER_SOFT_RST_DISABLE);
		/* Enables DRE in this path */
		snd_soc_component_update_bits(component, rx_path_cfg0_reg,
					WCD9335_CDC_RX_PATH_CFG_CMP_EN_MASK,
					WCD9335_CDC_RX_PATH_CFG_CMP_ENABLE);
	}

	if (SND_SOC_DAPM_EVENT_OFF(event)) {
		snd_soc_component_update_bits(component, comp_ctl0_reg,
					WCD9335_CDC_COMPANDER_HALT_MASK,
					WCD9335_CDC_COMPANDER_HALT);
		snd_soc_component_update_bits(component, rx_path_cfg0_reg,
					WCD9335_CDC_RX_PATH_CFG_CMP_EN_MASK,
					WCD9335_CDC_RX_PATH_CFG_CMP_DISABLE);

		snd_soc_component_update_bits(component, comp_ctl0_reg,
					WCD9335_CDC_COMPANDER_SOFT_RST_MASK,
					WCD9335_CDC_COMPANDER_SOFT_RST_ENABLE);
		snd_soc_component_update_bits(component, comp_ctl0_reg,
				WCD9335_CDC_COMPANDER_SOFT_RST_MASK,
				WCD9335_CDC_COMPANDER_SOFT_RST_DISABLE);
		snd_soc_component_update_bits(component, comp_ctl0_reg,
					WCD9335_CDC_COMPANDER_CLK_EN_MASK,
					WCD9335_CDC_COMPANDER_CLK_DISABLE);
		snd_soc_component_update_bits(component, comp_ctl0_reg,
					WCD9335_CDC_COMPANDER_HALT_MASK,
					WCD9335_CDC_COMPANDER_NOHALT);
	}

	return 0;
}