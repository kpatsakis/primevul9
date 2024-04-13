static int wcd9335_probe(struct wcd9335_codec *wcd)
{
	struct device *dev = wcd->dev;

	memcpy(wcd->rx_chs, wcd9335_rx_chs, sizeof(wcd9335_rx_chs));
	memcpy(wcd->tx_chs, wcd9335_tx_chs, sizeof(wcd9335_tx_chs));

	wcd->sido_input_src = SIDO_SOURCE_INTERNAL;
	wcd->sido_voltage = SIDO_VOLTAGE_NOMINAL_MV;

	return devm_snd_soc_register_component(dev, &wcd9335_component_drv,
					       wcd9335_slim_dais,
					       ARRAY_SIZE(wcd9335_slim_dais));
}