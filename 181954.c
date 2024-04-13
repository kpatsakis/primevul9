static int wcd9335_cdc_req_mclk_enable(struct wcd9335_codec *wcd,
				     bool enable)
{
	int ret = 0;

	if (enable) {
		wcd9335_cdc_sido_ccl_enable(wcd, true);
		ret = clk_prepare_enable(wcd->mclk);
		if (ret) {
			dev_err(wcd->dev, "%s: ext clk enable failed\n",
				__func__);
			goto err;
		}
		/* get BG */
		wcd9335_enable_master_bias(wcd);
		/* get MCLK */
		wcd9335_enable_mclk(wcd);

	} else {
		/* put MCLK */
		wcd9335_disable_mclk(wcd);
		/* put BG */
		wcd9335_disable_master_bias(wcd);
		clk_disable_unprepare(wcd->mclk);
		wcd9335_cdc_sido_ccl_enable(wcd, false);
	}
err:
	return ret;
}