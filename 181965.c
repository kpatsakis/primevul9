static int wcd9335_codec_hphr_dac_event(struct snd_soc_dapm_widget *w,
				      struct snd_kcontrol *kc,
				      int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);
	struct wcd9335_codec *wcd = dev_get_drvdata(comp->dev);
	int hph_mode = wcd->hph_mode;
	u8 dem_inp;
	int ret = 0;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:

		/* Read DEM INP Select */
		dem_inp = snd_soc_component_read32(comp,
				WCD9335_CDC_RX2_RX_PATH_SEC0) &
				WCD9335_CDC_RX_PATH_DEM_INP_SEL_MASK;
		if (((hph_mode == CLS_H_HIFI) || (hph_mode == CLS_H_LOHIFI) ||
		     (hph_mode == CLS_H_LP)) && (dem_inp != 0x01)) {
			dev_err(comp->dev, "DEM Input not set correctly, hph_mode: %d\n",
				hph_mode);
			return -EINVAL;
		}

		wcd_clsh_ctrl_set_state(wcd->clsh_ctrl,
			     WCD_CLSH_EVENT_PRE_DAC,
			     WCD_CLSH_STATE_HPHR,
			     ((hph_mode == CLS_H_LOHIFI) ?
			       CLS_H_HIFI : hph_mode));

		wcd9335_codec_hph_mode_config(comp, event, hph_mode);

		break;
	case SND_SOC_DAPM_POST_PMD:
		/* 1000us required as per HW requirement */
		usleep_range(1000, 1100);

		if (!(wcd_clsh_ctrl_get_state(wcd->clsh_ctrl) &
					WCD_CLSH_STATE_HPHL))
			wcd9335_codec_hph_mode_config(comp, event, hph_mode);

		wcd_clsh_ctrl_set_state(wcd->clsh_ctrl, WCD_CLSH_EVENT_POST_PA,
			     WCD_CLSH_STATE_HPHR, ((hph_mode == CLS_H_LOHIFI) ?
						CLS_H_HIFI : hph_mode));
		break;
	};

	return ret;
}