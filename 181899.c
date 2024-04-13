static int wcd9335_codec_enable_dec(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *kc, int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);
	unsigned int decimator;
	char *dec_adc_mux_name = NULL;
	char *widget_name = NULL;
	char *wname;
	int ret = 0, amic_n;
	u16 tx_vol_ctl_reg, pwr_level_reg = 0, dec_cfg_reg, hpf_gate_reg;
	u16 tx_gain_ctl_reg;
	char *dec;
	u8 hpf_coff_freq;

	widget_name = kmemdup_nul(w->name, 15, GFP_KERNEL);
	if (!widget_name)
		return -ENOMEM;

	wname = widget_name;
	dec_adc_mux_name = strsep(&widget_name, " ");
	if (!dec_adc_mux_name) {
		dev_err(comp->dev, "%s: Invalid decimator = %s\n",
			__func__, w->name);
		ret =  -EINVAL;
		goto out;
	}
	dec_adc_mux_name = widget_name;

	dec = strpbrk(dec_adc_mux_name, "012345678");
	if (!dec) {
		dev_err(comp->dev, "%s: decimator index not found\n",
			__func__);
		ret =  -EINVAL;
		goto out;
	}

	ret = kstrtouint(dec, 10, &decimator);
	if (ret < 0) {
		dev_err(comp->dev, "%s: Invalid decimator = %s\n",
			__func__, wname);
		ret =  -EINVAL;
		goto out;
	}

	tx_vol_ctl_reg = WCD9335_CDC_TX0_TX_PATH_CTL + 16 * decimator;
	hpf_gate_reg = WCD9335_CDC_TX0_TX_PATH_SEC2 + 16 * decimator;
	dec_cfg_reg = WCD9335_CDC_TX0_TX_PATH_CFG0 + 16 * decimator;
	tx_gain_ctl_reg = WCD9335_CDC_TX0_TX_VOL_CTL + 16 * decimator;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		amic_n = wcd9335_codec_find_amic_input(comp, decimator);
		if (amic_n)
			pwr_level_reg = wcd9335_codec_get_amic_pwlvl_reg(comp,
								       amic_n);

		if (pwr_level_reg) {
			switch ((snd_soc_component_read32(comp, pwr_level_reg) &
					      WCD9335_AMIC_PWR_LVL_MASK) >>
					      WCD9335_AMIC_PWR_LVL_SHIFT) {
			case WCD9335_AMIC_PWR_LEVEL_LP:
				snd_soc_component_update_bits(comp, dec_cfg_reg,
						    WCD9335_DEC_PWR_LVL_MASK,
						    WCD9335_DEC_PWR_LVL_LP);
				break;

			case WCD9335_AMIC_PWR_LEVEL_HP:
				snd_soc_component_update_bits(comp, dec_cfg_reg,
						    WCD9335_DEC_PWR_LVL_MASK,
						    WCD9335_DEC_PWR_LVL_HP);
				break;
			case WCD9335_AMIC_PWR_LEVEL_DEFAULT:
			default:
				snd_soc_component_update_bits(comp, dec_cfg_reg,
						    WCD9335_DEC_PWR_LVL_MASK,
						    WCD9335_DEC_PWR_LVL_DF);
				break;
			}
		}
		hpf_coff_freq = (snd_soc_component_read32(comp, dec_cfg_reg) &
				   TX_HPF_CUT_OFF_FREQ_MASK) >> 5;

		if (hpf_coff_freq != CF_MIN_3DB_150HZ)
			snd_soc_component_update_bits(comp, dec_cfg_reg,
					    TX_HPF_CUT_OFF_FREQ_MASK,
					    CF_MIN_3DB_150HZ << 5);
		/* Enable TX PGA Mute */
		snd_soc_component_update_bits(comp, tx_vol_ctl_reg,
						0x10, 0x10);
		/* Enable APC */
		snd_soc_component_update_bits(comp, dec_cfg_reg, 0x08, 0x08);
		break;
	case SND_SOC_DAPM_POST_PMU:
		snd_soc_component_update_bits(comp, hpf_gate_reg, 0x01, 0x00);

		if (decimator == 0) {
			snd_soc_component_write(comp,
					WCD9335_MBHC_ZDET_RAMP_CTL, 0x83);
			snd_soc_component_write(comp,
					WCD9335_MBHC_ZDET_RAMP_CTL, 0xA3);
			snd_soc_component_write(comp,
					WCD9335_MBHC_ZDET_RAMP_CTL, 0x83);
			snd_soc_component_write(comp,
					WCD9335_MBHC_ZDET_RAMP_CTL, 0x03);
		}

		snd_soc_component_update_bits(comp, hpf_gate_reg,
						0x01, 0x01);
		snd_soc_component_update_bits(comp, tx_vol_ctl_reg,
						0x10, 0x00);
		snd_soc_component_write(comp, tx_gain_ctl_reg,
			      snd_soc_component_read32(comp, tx_gain_ctl_reg));
		break;
	case SND_SOC_DAPM_PRE_PMD:
		hpf_coff_freq = (snd_soc_component_read32(comp, dec_cfg_reg) &
				   TX_HPF_CUT_OFF_FREQ_MASK) >> 5;
		snd_soc_component_update_bits(comp, tx_vol_ctl_reg, 0x10, 0x10);
		snd_soc_component_update_bits(comp, dec_cfg_reg, 0x08, 0x00);
			if (hpf_coff_freq != CF_MIN_3DB_150HZ) {
				snd_soc_component_update_bits(comp, dec_cfg_reg,
						    TX_HPF_CUT_OFF_FREQ_MASK,
						    hpf_coff_freq << 5);
			}
		break;
	case SND_SOC_DAPM_POST_PMD:
		snd_soc_component_update_bits(comp, tx_vol_ctl_reg, 0x10, 0x00);
		break;
	};
out:
	kfree(wname);
	return ret;
}