static int wcd9335_codec_update_sido_voltage(struct wcd9335_codec *wcd,
					     enum wcd9335_sido_voltage req_mv)
{
	int ret = 0;

	/* enable mclk before setting SIDO voltage */
	ret = wcd9335_cdc_req_mclk_enable(wcd, true);
	if (ret) {
		dev_err(wcd->dev, "Ext clk enable failed\n");
		goto err;
	}

	wcd9335_codec_apply_sido_voltage(wcd, req_mv);
	wcd9335_cdc_req_mclk_enable(wcd, false);

err:
	return ret;
}