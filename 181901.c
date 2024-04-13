static int wcd9335_codec_enable_hphr_pa(struct snd_soc_dapm_widget *w,
					struct snd_kcontrol *kc, int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);
	struct wcd9335_codec *wcd = dev_get_drvdata(comp->dev);
	int hph_mode = wcd->hph_mode;
	int ret = 0;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		break;
	case SND_SOC_DAPM_POST_PMU:
		/*
		 * 7ms sleep is required after PA is enabled as per
		 * HW requirement
		 */
		usleep_range(7000, 7100);
		wcd9335_codec_hph_post_pa_config(wcd, hph_mode, event);
		snd_soc_component_update_bits(comp,
					WCD9335_CDC_RX2_RX_PATH_CTL,
					WCD9335_CDC_RX_PGA_MUTE_EN_MASK,
					WCD9335_CDC_RX_PGA_MUTE_DISABLE);
		/* Remove mix path mute if it is enabled */
		if ((snd_soc_component_read32(comp,
					WCD9335_CDC_RX2_RX_PATH_MIX_CTL)) &
					WCD9335_CDC_RX_PGA_MUTE_EN_MASK)
			snd_soc_component_update_bits(comp,
					WCD9335_CDC_RX2_RX_PATH_MIX_CTL,
					WCD9335_CDC_RX_PGA_MUTE_EN_MASK,
					WCD9335_CDC_RX_PGA_MUTE_DISABLE);

		break;

	case SND_SOC_DAPM_PRE_PMD:
		wcd9335_codec_hph_post_pa_config(wcd, hph_mode, event);
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