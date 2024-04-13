static int wcd9335_get_compander(struct snd_kcontrol *kc,
			       struct snd_ctl_elem_value *ucontrol)
{

	struct snd_soc_component *component = snd_soc_kcontrol_component(kc);
	int comp = ((struct soc_mixer_control *)kc->private_value)->shift;
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);

	ucontrol->value.integer.value[0] = wcd->comp_enabled[comp];
	return 0;
}