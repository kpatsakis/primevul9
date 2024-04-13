static int wcd9335_codec_enable_dmic(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kc, int event)
{
	struct snd_soc_component *comp = snd_soc_dapm_to_component(w->dapm);
	struct wcd9335_codec *wcd = snd_soc_component_get_drvdata(comp);
	u8  dmic_clk_en = 0x01;
	u16 dmic_clk_reg;
	s32 *dmic_clk_cnt;
	u8 dmic_rate_val, dmic_rate_shift = 1;
	unsigned int dmic;
	int ret;
	char *wname;

	wname = strpbrk(w->name, "012345");
	if (!wname) {
		dev_err(comp->dev, "%s: widget not found\n", __func__);
		return -EINVAL;
	}

	ret = kstrtouint(wname, 10, &dmic);
	if (ret < 0) {
		dev_err(comp->dev, "%s: Invalid DMIC line on the codec\n",
			__func__);
		return -EINVAL;
	}

	switch (dmic) {
	case 0:
	case 1:
		dmic_clk_cnt = &(wcd->dmic_0_1_clk_cnt);
		dmic_clk_reg = WCD9335_CPE_SS_DMIC0_CTL;
		break;
	case 2:
	case 3:
		dmic_clk_cnt = &(wcd->dmic_2_3_clk_cnt);
		dmic_clk_reg = WCD9335_CPE_SS_DMIC1_CTL;
		break;
	case 4:
	case 5:
		dmic_clk_cnt = &(wcd->dmic_4_5_clk_cnt);
		dmic_clk_reg = WCD9335_CPE_SS_DMIC2_CTL;
		break;
	default:
		dev_err(comp->dev, "%s: Invalid DMIC Selection\n",
			__func__);
		return -EINVAL;
	};

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		dmic_rate_val =
			wcd9335_get_dmic_clk_val(comp,
					wcd->mclk_rate,
					wcd->dmic_sample_rate);

		(*dmic_clk_cnt)++;
		if (*dmic_clk_cnt == 1) {
			snd_soc_component_update_bits(comp, dmic_clk_reg,
				0x07 << dmic_rate_shift,
				dmic_rate_val << dmic_rate_shift);
			snd_soc_component_update_bits(comp, dmic_clk_reg,
					dmic_clk_en, dmic_clk_en);
		}

		break;
	case SND_SOC_DAPM_POST_PMD:
		dmic_rate_val =
			wcd9335_get_dmic_clk_val(comp,
					wcd->mclk_rate,
					wcd->mad_dmic_sample_rate);
		(*dmic_clk_cnt)--;
		if (*dmic_clk_cnt  == 0) {
			snd_soc_component_update_bits(comp, dmic_clk_reg,
					dmic_clk_en, 0);
			snd_soc_component_update_bits(comp, dmic_clk_reg,
				0x07 << dmic_rate_shift,
				dmic_rate_val << dmic_rate_shift);
		}
		break;
	};

	return 0;
}