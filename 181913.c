static int wcd9335_put_dec_enum(struct snd_kcontrol *kc,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_dapm_context *dapm = snd_soc_dapm_kcontrol_dapm(kc);
	struct snd_soc_component *component = snd_soc_dapm_to_component(dapm);
	struct soc_enum *e = (struct soc_enum *)kc->private_value;
	unsigned int val, reg, sel;

	val = ucontrol->value.enumerated.item[0];

	switch (e->reg) {
	case WCD9335_CDC_TX_INP_MUX_ADC_MUX0_CFG1:
		reg = WCD9335_CDC_TX0_TX_PATH_CFG0;
		break;
	case WCD9335_CDC_TX_INP_MUX_ADC_MUX1_CFG1:
		reg = WCD9335_CDC_TX1_TX_PATH_CFG0;
		break;
	case WCD9335_CDC_TX_INP_MUX_ADC_MUX2_CFG1:
		reg = WCD9335_CDC_TX2_TX_PATH_CFG0;
		break;
	case WCD9335_CDC_TX_INP_MUX_ADC_MUX3_CFG1:
		reg = WCD9335_CDC_TX3_TX_PATH_CFG0;
		break;
	case WCD9335_CDC_TX_INP_MUX_ADC_MUX4_CFG0:
		reg = WCD9335_CDC_TX4_TX_PATH_CFG0;
		break;
	case WCD9335_CDC_TX_INP_MUX_ADC_MUX5_CFG0:
		reg = WCD9335_CDC_TX5_TX_PATH_CFG0;
		break;
	case WCD9335_CDC_TX_INP_MUX_ADC_MUX6_CFG0:
		reg = WCD9335_CDC_TX6_TX_PATH_CFG0;
		break;
	case WCD9335_CDC_TX_INP_MUX_ADC_MUX7_CFG0:
		reg = WCD9335_CDC_TX7_TX_PATH_CFG0;
		break;
	case WCD9335_CDC_TX_INP_MUX_ADC_MUX8_CFG0:
		reg = WCD9335_CDC_TX8_TX_PATH_CFG0;
		break;
	default:
		return -EINVAL;
	}

	/* AMIC: 0, DMIC: 1 */
	sel = val ? WCD9335_CDC_TX_ADC_AMIC_SEL : WCD9335_CDC_TX_ADC_DMIC_SEL;
	snd_soc_component_update_bits(component, reg,
				      WCD9335_CDC_TX_ADC_AMIC_DMIC_SEL_MASK,
				      sel);

	return snd_soc_dapm_put_enum_double(kc, ucontrol);
}