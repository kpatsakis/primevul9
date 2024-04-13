static int wcd9335_int_dem_inp_mux_put(struct snd_kcontrol *kc,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct soc_enum *e = (struct soc_enum *)kc->private_value;
	struct snd_soc_component *component;
	int reg, val;

	component = snd_soc_dapm_kcontrol_component(kc);
	val = ucontrol->value.enumerated.item[0];

	if (e->reg == WCD9335_CDC_RX0_RX_PATH_SEC0)
		reg = WCD9335_CDC_RX0_RX_PATH_CFG0;
	else if (e->reg == WCD9335_CDC_RX1_RX_PATH_SEC0)
		reg = WCD9335_CDC_RX1_RX_PATH_CFG0;
	else if (e->reg == WCD9335_CDC_RX2_RX_PATH_SEC0)
		reg = WCD9335_CDC_RX2_RX_PATH_CFG0;
	else
		return -EINVAL;

	/* Set Look Ahead Delay */
	snd_soc_component_update_bits(component, reg,
				WCD9335_CDC_RX_PATH_CFG0_DLY_ZN_EN_MASK,
				val ? WCD9335_CDC_RX_PATH_CFG0_DLY_ZN_EN : 0);
	/* Set DEM INP Select */
	return snd_soc_dapm_put_enum_double(kc, ucontrol);
}