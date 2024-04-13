static int wcd9335_rx_hph_mode_get(struct snd_kcontrol *kc,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kc);
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);

	ucontrol->value.enumerated.item[0] = wcd->hph_mode;

	return 0;
}