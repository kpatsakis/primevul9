static int slim_rx_mux_get(struct snd_kcontrol *kc,
			   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_dapm_context *dapm = snd_soc_dapm_kcontrol_dapm(kc);
	struct wcd9335_codec *wcd = dev_get_drvdata(dapm->dev);

	ucontrol->value.enumerated.item[0] = wcd->rx_port_value;

	return 0;
}