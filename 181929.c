static int wcd9335_codec_enable_mclk(struct snd_soc_dapm_widget *w,
				     struct snd_kcontrol *kc, int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		return _wcd9335_codec_enable_mclk(comp, true);
	case SND_SOC_DAPM_POST_PMD:
		return _wcd9335_codec_enable_mclk(comp, false);
	}

	return 0;
}