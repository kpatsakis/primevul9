static int slim_tx_mixer_get(struct snd_kcontrol *kc,
			     struct snd_ctl_elem_value *ucontrol)
{

	struct snd_soc_dapm_context *dapm = snd_soc_dapm_kcontrol_dapm(kc);
	struct wcd9335_codec *wcd = dev_get_drvdata(dapm->dev);

	ucontrol->value.integer.value[0] = wcd->tx_port_value;

	return 0;
}