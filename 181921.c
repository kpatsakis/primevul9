static void wcd9335_codec_hph_mode_config(struct snd_soc_component *component,
					  int event, int mode)
{
	switch (mode) {
	case CLS_H_LP:
		wcd9335_codec_hph_lp_config(component, event);
		break;
	case CLS_H_LOHIFI:
		wcd9335_codec_hph_lohifi_config(component, event);
		break;
	case CLS_H_HIFI:
		wcd9335_codec_hph_hifi_config(component, event);
		break;
	}
}