static int wcd9335_codec_enable_adc(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kc, int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wcd9335_codec_set_tx_hold(comp, w->reg, true);
		break;
	default:
		break;
	}

	return 0;
}