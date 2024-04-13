static int _wcd9335_codec_enable_mclk(struct snd_soc_component *component,
				      int enable)
{
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);
	int ret;

	if (enable) {
		ret = wcd9335_cdc_req_mclk_enable(wcd, true);
		if (ret)
			return ret;

		wcd9335_codec_apply_sido_voltage(wcd,
				SIDO_VOLTAGE_NOMINAL_MV);
	} else {
		wcd9335_codec_update_sido_voltage(wcd,
					wcd->sido_voltage);
		wcd9335_cdc_req_mclk_enable(wcd, false);
	}

	return 0;
}