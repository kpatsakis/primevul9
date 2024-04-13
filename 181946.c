static void wcd9335_codec_hd2_control(struct snd_soc_component *component,
				    u16 prim_int_reg, int event)
{
	u16 hd2_scale_reg;
	u16 hd2_enable_reg = 0;

	if (prim_int_reg == WCD9335_CDC_RX1_RX_PATH_CTL) {
		hd2_scale_reg = WCD9335_CDC_RX1_RX_PATH_SEC3;
		hd2_enable_reg = WCD9335_CDC_RX1_RX_PATH_CFG0;
	}
	if (prim_int_reg == WCD9335_CDC_RX2_RX_PATH_CTL) {
		hd2_scale_reg = WCD9335_CDC_RX2_RX_PATH_SEC3;
		hd2_enable_reg = WCD9335_CDC_RX2_RX_PATH_CFG0;
	}

	if (hd2_enable_reg && SND_SOC_DAPM_EVENT_ON(event)) {
		snd_soc_component_update_bits(component, hd2_scale_reg,
				WCD9335_CDC_RX_PATH_SEC_HD2_ALPHA_MASK,
				WCD9335_CDC_RX_PATH_SEC_HD2_ALPHA_0P2500);
		snd_soc_component_update_bits(component, hd2_scale_reg,
				WCD9335_CDC_RX_PATH_SEC_HD2_SCALE_MASK,
				WCD9335_CDC_RX_PATH_SEC_HD2_SCALE_2);
		snd_soc_component_update_bits(component, hd2_enable_reg,
				WCD9335_CDC_RX_PATH_CFG_HD2_EN_MASK,
				WCD9335_CDC_RX_PATH_CFG_HD2_ENABLE);
	}

	if (hd2_enable_reg && SND_SOC_DAPM_EVENT_OFF(event)) {
		snd_soc_component_update_bits(component, hd2_enable_reg,
					WCD9335_CDC_RX_PATH_CFG_HD2_EN_MASK,
					WCD9335_CDC_RX_PATH_CFG_HD2_DISABLE);
		snd_soc_component_update_bits(component, hd2_scale_reg,
					WCD9335_CDC_RX_PATH_SEC_HD2_SCALE_MASK,
					WCD9335_CDC_RX_PATH_SEC_HD2_SCALE_1);
		snd_soc_component_update_bits(component, hd2_scale_reg,
				WCD9335_CDC_RX_PATH_SEC_HD2_ALPHA_MASK,
				WCD9335_CDC_RX_PATH_SEC_HD2_ALPHA_0P0000);
	}
}