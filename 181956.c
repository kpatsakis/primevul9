static void wcd9335_cdc_sido_ccl_enable(struct wcd9335_codec *wcd,
					bool ccl_flag)
{
	struct snd_soc_component *comp = wcd->component;

	if (ccl_flag) {
		if (++wcd->sido_ccl_cnt == 1)
			snd_soc_component_write(comp, WCD9335_SIDO_SIDO_CCL_10,
					WCD9335_SIDO_SIDO_CCL_DEF_VALUE);
	} else {
		if (wcd->sido_ccl_cnt == 0) {
			dev_err(wcd->dev, "sido_ccl already disabled\n");
			return;
		}
		if (--wcd->sido_ccl_cnt == 0)
			snd_soc_component_write(comp, WCD9335_SIDO_SIDO_CCL_10,
				WCD9335_SIDO_SIDO_CCL_10_ICHARG_PWR_SEL_C320FF);
	}
}