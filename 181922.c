static int wcd9335_codec_enable_mix_path(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kc, int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);
	u16 gain_reg;
	int offset_val = 0;
	int val = 0;

	switch (w->reg) {
	case WCD9335_CDC_RX0_RX_PATH_MIX_CTL:
		gain_reg = WCD9335_CDC_RX0_RX_VOL_MIX_CTL;
		break;
	case WCD9335_CDC_RX1_RX_PATH_MIX_CTL:
		gain_reg = WCD9335_CDC_RX1_RX_VOL_MIX_CTL;
		break;
	case WCD9335_CDC_RX2_RX_PATH_MIX_CTL:
		gain_reg = WCD9335_CDC_RX2_RX_VOL_MIX_CTL;
		break;
	case WCD9335_CDC_RX3_RX_PATH_MIX_CTL:
		gain_reg = WCD9335_CDC_RX3_RX_VOL_MIX_CTL;
		break;
	case WCD9335_CDC_RX4_RX_PATH_MIX_CTL:
		gain_reg = WCD9335_CDC_RX4_RX_VOL_MIX_CTL;
		break;
	case WCD9335_CDC_RX5_RX_PATH_MIX_CTL:
		gain_reg = WCD9335_CDC_RX5_RX_VOL_MIX_CTL;
		break;
	case WCD9335_CDC_RX6_RX_PATH_MIX_CTL:
		gain_reg = WCD9335_CDC_RX6_RX_VOL_MIX_CTL;
		break;
	case WCD9335_CDC_RX7_RX_PATH_MIX_CTL:
		gain_reg = WCD9335_CDC_RX7_RX_VOL_MIX_CTL;
		break;
	case WCD9335_CDC_RX8_RX_PATH_MIX_CTL:
		gain_reg = WCD9335_CDC_RX8_RX_VOL_MIX_CTL;
		break;
	default:
		dev_err(comp->dev, "%s: No gain register avail for %s\n",
			__func__, w->name);
		return 0;
	};

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		val = snd_soc_component_read32(comp, gain_reg);
		val += offset_val;
		snd_soc_component_write(comp, gain_reg, val);
		break;
	case SND_SOC_DAPM_POST_PMD:
		break;
	};

	return 0;
}