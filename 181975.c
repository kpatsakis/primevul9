static int wcd9335_set_channel_map(struct snd_soc_dai *dai,
				   unsigned int tx_num, unsigned int *tx_slot,
				   unsigned int rx_num, unsigned int *rx_slot)
{
	struct wcd9335_codec *wcd;
	int i;

	wcd = snd_soc_component_get_drvdata(dai->component);

	if (!tx_slot || !rx_slot) {
		dev_err(wcd->dev, "Invalid tx_slot=%p, rx_slot=%p\n",
			tx_slot, rx_slot);
		return -EINVAL;
	}

	wcd->num_rx_port = rx_num;
	for (i = 0; i < rx_num; i++) {
		wcd->rx_chs[i].ch_num = rx_slot[i];
		INIT_LIST_HEAD(&wcd->rx_chs[i].list);
	}

	wcd->num_tx_port = tx_num;
	for (i = 0; i < tx_num; i++) {
		wcd->tx_chs[i].ch_num = tx_slot[i];
		INIT_LIST_HEAD(&wcd->tx_chs[i].list);
	}

	return 0;
}