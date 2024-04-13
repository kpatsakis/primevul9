static int wcd9335_set_compander(struct snd_kcontrol *kc,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kc);
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);
	int comp = ((struct soc_mixer_control *) kc->private_value)->shift;
	int value = ucontrol->value.integer.value[0];
	int sel;

	wcd->comp_enabled[comp] = value;
	sel = value ? WCD9335_HPH_GAIN_SRC_SEL_COMPANDER :
		WCD9335_HPH_GAIN_SRC_SEL_REGISTER;

	/* Any specific register configuration for compander */
	switch (comp) {
	case COMPANDER_1:
		/* Set Gain Source Select based on compander enable/disable */
		snd_soc_component_update_bits(component, WCD9335_HPH_L_EN,
				      WCD9335_HPH_GAIN_SRC_SEL_MASK, sel);
		break;
	case COMPANDER_2:
		snd_soc_component_update_bits(component, WCD9335_HPH_R_EN,
				      WCD9335_HPH_GAIN_SRC_SEL_MASK, sel);
		break;
	case COMPANDER_5:
		snd_soc_component_update_bits(component, WCD9335_SE_LO_LO3_GAIN,
				      WCD9335_HPH_GAIN_SRC_SEL_MASK, sel);
		break;
	case COMPANDER_6:
		snd_soc_component_update_bits(component, WCD9335_SE_LO_LO4_GAIN,
				      WCD9335_HPH_GAIN_SRC_SEL_MASK, sel);
		break;
	default:
		break;
	};

	return 0;
}