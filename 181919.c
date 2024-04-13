static int wcd9335_micbias_control(struct snd_soc_component *component,
				   int micb_num, int req, bool is_dapm)
{
	struct wcd9335_codec *wcd = snd_soc_component_get_drvdata(component);
	int micb_index = micb_num - 1;
	u16 micb_reg;

	if ((micb_index < 0) || (micb_index > WCD9335_MAX_MICBIAS - 1)) {
		dev_err(wcd->dev, "Invalid micbias index, micb_ind:%d\n",
			micb_index);
		return -EINVAL;
	}

	switch (micb_num) {
	case MIC_BIAS_1:
		micb_reg = WCD9335_ANA_MICB1;
		break;
	case MIC_BIAS_2:
		micb_reg = WCD9335_ANA_MICB2;
		break;
	case MIC_BIAS_3:
		micb_reg = WCD9335_ANA_MICB3;
		break;
	case MIC_BIAS_4:
		micb_reg = WCD9335_ANA_MICB4;
		break;
	default:
		dev_err(component->dev, "%s: Invalid micbias number: %d\n",
			__func__, micb_num);
		return -EINVAL;
	}

	switch (req) {
	case MICB_PULLUP_ENABLE:
		wcd->pullup_ref[micb_index]++;
		if ((wcd->pullup_ref[micb_index] == 1) &&
		    (wcd->micb_ref[micb_index] == 0))
			snd_soc_component_update_bits(component, micb_reg,
							0xC0, 0x80);
		break;
	case MICB_PULLUP_DISABLE:
		wcd->pullup_ref[micb_index]--;
		if ((wcd->pullup_ref[micb_index] == 0) &&
		    (wcd->micb_ref[micb_index] == 0))
			snd_soc_component_update_bits(component, micb_reg,
							0xC0, 0x00);
		break;
	case MICB_ENABLE:
		wcd->micb_ref[micb_index]++;
		if (wcd->micb_ref[micb_index] == 1)
			snd_soc_component_update_bits(component, micb_reg,
							0xC0, 0x40);
		break;
	case MICB_DISABLE:
		wcd->micb_ref[micb_index]--;
		if ((wcd->micb_ref[micb_index] == 0) &&
		    (wcd->pullup_ref[micb_index] > 0))
			snd_soc_component_update_bits(component, micb_reg,
							0xC0, 0x80);
		else if ((wcd->micb_ref[micb_index] == 0) &&
			 (wcd->pullup_ref[micb_index] == 0)) {
			snd_soc_component_update_bits(component, micb_reg,
							0xC0, 0x00);
		}
		break;
	};

	return 0;
}