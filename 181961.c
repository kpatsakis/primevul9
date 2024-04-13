static int wcd9335_codec_enable_interpolator(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kc, int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);
	u16 gain_reg;
	u16 reg;
	int val;
	int offset_val = 0;

	if (!(strcmp(w->name, "RX INT0 INTERP"))) {
		reg = WCD9335_CDC_RX0_RX_PATH_CTL;
		gain_reg = WCD9335_CDC_RX0_RX_VOL_CTL;
	} else if (!(strcmp(w->name, "RX INT1 INTERP"))) {
		reg = WCD9335_CDC_RX1_RX_PATH_CTL;
		gain_reg = WCD9335_CDC_RX1_RX_VOL_CTL;
	} else if (!(strcmp(w->name, "RX INT2 INTERP"))) {
		reg = WCD9335_CDC_RX2_RX_PATH_CTL;
		gain_reg = WCD9335_CDC_RX2_RX_VOL_CTL;
	} else if (!(strcmp(w->name, "RX INT3 INTERP"))) {
		reg = WCD9335_CDC_RX3_RX_PATH_CTL;
		gain_reg = WCD9335_CDC_RX3_RX_VOL_CTL;
	} else if (!(strcmp(w->name, "RX INT4 INTERP"))) {
		reg = WCD9335_CDC_RX4_RX_PATH_CTL;
		gain_reg = WCD9335_CDC_RX4_RX_VOL_CTL;
	} else if (!(strcmp(w->name, "RX INT5 INTERP"))) {
		reg = WCD9335_CDC_RX5_RX_PATH_CTL;
		gain_reg = WCD9335_CDC_RX5_RX_VOL_CTL;
	} else if (!(strcmp(w->name, "RX INT6 INTERP"))) {
		reg = WCD9335_CDC_RX6_RX_PATH_CTL;
		gain_reg = WCD9335_CDC_RX6_RX_VOL_CTL;
	} else if (!(strcmp(w->name, "RX INT7 INTERP"))) {
		reg = WCD9335_CDC_RX7_RX_PATH_CTL;
		gain_reg = WCD9335_CDC_RX7_RX_VOL_CTL;
	} else if (!(strcmp(w->name, "RX INT8 INTERP"))) {
		reg = WCD9335_CDC_RX8_RX_PATH_CTL;
		gain_reg = WCD9335_CDC_RX8_RX_VOL_CTL;
	} else {
		dev_err(comp->dev, "%s: Interpolator reg not found\n",
			__func__);
		return -EINVAL;
	}

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* Reset if needed */
		wcd9335_codec_enable_prim_interpolator(comp, reg, event);
		break;
	case SND_SOC_DAPM_POST_PMU:
		wcd9335_config_compander(comp, w->shift, event);
		val = snd_soc_component_read32(comp, gain_reg);
		val += offset_val;
		snd_soc_component_write(comp, gain_reg, val);
		break;
	case SND_SOC_DAPM_POST_PMD:
		wcd9335_config_compander(comp, w->shift, event);
		wcd9335_codec_enable_prim_interpolator(comp, reg, event);
		break;
	};

	return 0;
}