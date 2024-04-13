static int wcd9335_enable_master_bias(struct wcd9335_codec *wcd)
{
	wcd->master_bias_users++;
	if (wcd->master_bias_users == 1) {
		regmap_update_bits(wcd->regmap, WCD9335_ANA_BIAS,
					WCD9335_ANA_BIAS_EN_MASK,
					WCD9335_ANA_BIAS_ENABLE);
		regmap_update_bits(wcd->regmap, WCD9335_ANA_BIAS,
					WCD9335_ANA_BIAS_PRECHRG_EN_MASK,
					WCD9335_ANA_BIAS_PRECHRG_ENABLE);
		/*
		 * 1ms delay is required after pre-charge is enabled
		 * as per HW requirement
		 */
		usleep_range(1000, 1100);
		regmap_update_bits(wcd->regmap, WCD9335_ANA_BIAS,
					WCD9335_ANA_BIAS_PRECHRG_EN_MASK,
					WCD9335_ANA_BIAS_PRECHRG_DISABLE);
		regmap_update_bits(wcd->regmap, WCD9335_ANA_BIAS,
				WCD9335_ANA_BIAS_PRECHRG_CTL_MODE,
				WCD9335_ANA_BIAS_PRECHRG_CTL_MODE_MANUAL);
	}

	return 0;
}