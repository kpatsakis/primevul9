static u8 wcd9335_get_dmic_clk_val(struct snd_soc_component *component,
				 u32 mclk_rate, u32 dmic_clk_rate)
{
	u32 div_factor;
	u8 dmic_ctl_val;

	dev_err(component->dev,
		"%s: mclk_rate = %d, dmic_sample_rate = %d\n",
		__func__, mclk_rate, dmic_clk_rate);

	/* Default value to return in case of error */
	if (mclk_rate == WCD9335_MCLK_CLK_9P6MHZ)
		dmic_ctl_val = WCD9335_DMIC_CLK_DIV_2;
	else
		dmic_ctl_val = WCD9335_DMIC_CLK_DIV_3;

	if (dmic_clk_rate == 0) {
		dev_err(component->dev,
			"%s: dmic_sample_rate cannot be 0\n",
			__func__);
		goto done;
	}

	div_factor = mclk_rate / dmic_clk_rate;
	switch (div_factor) {
	case 2:
		dmic_ctl_val = WCD9335_DMIC_CLK_DIV_2;
		break;
	case 3:
		dmic_ctl_val = WCD9335_DMIC_CLK_DIV_3;
		break;
	case 4:
		dmic_ctl_val = WCD9335_DMIC_CLK_DIV_4;
		break;
	case 6:
		dmic_ctl_val = WCD9335_DMIC_CLK_DIV_6;
		break;
	case 8:
		dmic_ctl_val = WCD9335_DMIC_CLK_DIV_8;
		break;
	case 16:
		dmic_ctl_val = WCD9335_DMIC_CLK_DIV_16;
		break;
	default:
		dev_err(component->dev,
			"%s: Invalid div_factor %u, clk_rate(%u), dmic_rate(%u)\n",
			__func__, div_factor, mclk_rate, dmic_clk_rate);
		break;
	}

done:
	return dmic_ctl_val;
}