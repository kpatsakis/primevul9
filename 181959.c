static int wcd9335_codec_enable_slim(struct snd_soc_dapm_widget *w,
				       struct snd_kcontrol *kc,
				       int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);
	struct wcd9335_codec *wcd = snd_soc_component_get_drvdata(comp);
	struct wcd_slim_codec_dai_data *dai = &wcd->dai[w->shift];
	int ret = 0;

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		wcd9335_codec_enable_int_port(dai, comp);
		break;
	case SND_SOC_DAPM_POST_PMD:
		kfree(dai->sconfig.chs);

		break;
	}

	return ret;
}