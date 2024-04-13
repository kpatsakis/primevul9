static int wcd9335_rx_hph_mode_put(struct snd_kcontrol *kc,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kc);
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);
	u32 mode_val;

	mode_val = ucontrol->value.enumerated.item[0];

	if (mode_val == 0) {
		dev_err(wcd->dev, "Invalid HPH Mode, default to ClSH HiFi\n");
		mode_val = CLS_H_HIFI;
	}
	wcd->hph_mode = mode_val;

	return 0;
}