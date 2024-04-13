static int wcd9335_codec_enable_rx_bias(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kc, int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);
	struct wcd9335_codec *wcd = dev_get_drvdata(comp->dev);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wcd->rx_bias_count++;
		if (wcd->rx_bias_count == 1) {
			wcd9335_codec_init_flyback(comp);
			snd_soc_component_update_bits(comp,
						WCD9335_ANA_RX_SUPPLIES,
						WCD9335_ANA_RX_BIAS_ENABLE_MASK,
						WCD9335_ANA_RX_BIAS_ENABLE);
		}
		break;
	case SND_SOC_DAPM_POST_PMD:
		wcd->rx_bias_count--;
		if (!wcd->rx_bias_count)
			snd_soc_component_update_bits(comp,
					WCD9335_ANA_RX_SUPPLIES,
					WCD9335_ANA_RX_BIAS_ENABLE_MASK,
					WCD9335_ANA_RX_BIAS_DISABLE);
		break;
	};

	return 0;
}