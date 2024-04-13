static void wcd9335_codec_apply_sido_voltage(struct wcd9335_codec *wcd,
					     enum wcd9335_sido_voltage req_mv)
{
	struct snd_soc_component *comp = wcd->component;
	int vout_d_val;

	if (req_mv == wcd->sido_voltage)
		return;

	/* compute the vout_d step value */
	vout_d_val = WCD9335_CALCULATE_VOUT_D(req_mv) &
			WCD9335_ANA_BUCK_VOUT_MASK;
	snd_soc_component_write(comp, WCD9335_ANA_BUCK_VOUT_D, vout_d_val);
	snd_soc_component_update_bits(comp, WCD9335_ANA_BUCK_CTL,
				WCD9335_ANA_BUCK_CTL_RAMP_START_MASK,
				WCD9335_ANA_BUCK_CTL_RAMP_START_ENABLE);

	/* 1 msec sleep required after SIDO Vout_D voltage change */
	usleep_range(1000, 1100);
	wcd->sido_voltage = req_mv;
	snd_soc_component_update_bits(comp, WCD9335_ANA_BUCK_CTL,
				WCD9335_ANA_BUCK_CTL_RAMP_START_MASK,
				WCD9335_ANA_BUCK_CTL_RAMP_START_DISABLE);
}