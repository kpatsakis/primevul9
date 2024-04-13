static int ad5755_setup_pdata(struct iio_dev *indio_dev,
			      const struct ad5755_platform_data *pdata)
{
	struct ad5755_state *st = iio_priv(indio_dev);
	unsigned int val;
	unsigned int i;
	int ret;

	if (pdata->dc_dc_phase > AD5755_DC_DC_PHASE_90_DEGREE ||
		pdata->dc_dc_freq > AD5755_DC_DC_FREQ_650kHZ ||
		pdata->dc_dc_maxv > AD5755_DC_DC_MAXV_29V5)
		return -EINVAL;

	val = pdata->dc_dc_maxv << AD5755_DC_DC_MAXV;
	val |= pdata->dc_dc_freq << AD5755_DC_DC_FREQ_SHIFT;
	val |= pdata->dc_dc_phase << AD5755_DC_DC_PHASE_SHIFT;
	if (pdata->ext_dc_dc_compenstation_resistor)
		val |= AD5755_EXT_DC_DC_COMP_RES;

	ret = ad5755_write_ctrl(indio_dev, 0, AD5755_CTRL_REG_DC_DC, val);
	if (ret < 0)
		return ret;

	for (i = 0; i < ARRAY_SIZE(pdata->dac); ++i) {
		val = pdata->dac[i].slew.step_size <<
			AD5755_SLEW_STEP_SIZE_SHIFT;
		val |= pdata->dac[i].slew.rate <<
			AD5755_SLEW_RATE_SHIFT;
		if (pdata->dac[i].slew.enable)
			val |= AD5755_SLEW_ENABLE;

		ret = ad5755_write_ctrl(indio_dev, i,
					AD5755_CTRL_REG_SLEW, val);
		if (ret < 0)
			return ret;
	}

	for (i = 0; i < ARRAY_SIZE(pdata->dac); ++i) {
		if (!ad5755_is_valid_mode(st, pdata->dac[i].mode))
			return -EINVAL;

		val = 0;
		if (!pdata->dac[i].ext_current_sense_resistor)
			val |= AD5755_DAC_INT_CURRENT_SENSE_RESISTOR;
		if (pdata->dac[i].enable_voltage_overrange)
			val |= AD5755_DAC_VOLTAGE_OVERRANGE_EN;
		val |= pdata->dac[i].mode;

		ret = ad5755_update_dac_ctrl(indio_dev, i, val, 0);
		if (ret < 0)
			return ret;
	}

	return 0;
}