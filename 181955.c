static int wcd9335_codec_enable_lineout_pa(struct snd_soc_dapm_widget *w,
					 struct snd_kcontrol *kc,
					 int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);
	int vol_reg = 0, mix_vol_reg = 0;
	int ret = 0;

	if (w->reg == WCD9335_ANA_LO_1_2) {
		if (w->shift == 7) {
			vol_reg = WCD9335_CDC_RX3_RX_PATH_CTL;
			mix_vol_reg = WCD9335_CDC_RX3_RX_PATH_MIX_CTL;
		} else if (w->shift == 6) {
			vol_reg = WCD9335_CDC_RX4_RX_PATH_CTL;
			mix_vol_reg = WCD9335_CDC_RX4_RX_PATH_MIX_CTL;
		}
	} else if (w->reg == WCD9335_ANA_LO_3_4) {
		if (w->shift == 7) {
			vol_reg = WCD9335_CDC_RX5_RX_PATH_CTL;
			mix_vol_reg = WCD9335_CDC_RX5_RX_PATH_MIX_CTL;
		} else if (w->shift == 6) {
			vol_reg = WCD9335_CDC_RX6_RX_PATH_CTL;
			mix_vol_reg = WCD9335_CDC_RX6_RX_PATH_MIX_CTL;
		}
	} else {
		dev_err(comp->dev, "Error enabling lineout PA\n");
		return -EINVAL;
	}

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		/* 5ms sleep is required after PA is enabled as per
		 * HW requirement
		 */
		usleep_range(5000, 5500);
		snd_soc_component_update_bits(comp, vol_reg,
					WCD9335_CDC_RX_PGA_MUTE_EN_MASK,
					WCD9335_CDC_RX_PGA_MUTE_DISABLE);

		/* Remove mix path mute if it is enabled */
		if ((snd_soc_component_read32(comp, mix_vol_reg)) &
					WCD9335_CDC_RX_PGA_MUTE_EN_MASK)
			snd_soc_component_update_bits(comp,  mix_vol_reg,
					WCD9335_CDC_RX_PGA_MUTE_EN_MASK,
					WCD9335_CDC_RX_PGA_MUTE_DISABLE);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* 5ms sleep is required after PA is disabled as per
		 * HW requirement
		 */
		usleep_range(5000, 5500);
		break;
	};

	return ret;
}