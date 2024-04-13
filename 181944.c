static int wcd9335_set_decimator_rate(struct snd_soc_dai *dai,
				      u8 rate_val, u32 rate)
{
	struct snd_soc_component *comp = dai->component;
	struct wcd9335_codec *wcd = snd_soc_component_get_drvdata(comp);
	u8 shift = 0, shift_val = 0, tx_mux_sel;
	struct wcd9335_slim_ch *ch;
	int tx_port, tx_port_reg;
	int decimator = -1;

	list_for_each_entry(ch, &wcd->dai[dai->id].slim_ch_list, list) {
		tx_port = ch->port;
		if ((tx_port == 12) || (tx_port >= 14)) {
			dev_err(wcd->dev, "Invalid SLIM TX%u port DAI ID:%d\n",
				tx_port, dai->id);
			return -EINVAL;
		}
		/* Find the SB TX MUX input - which decimator is connected */
		if (tx_port < 4) {
			tx_port_reg = WCD9335_CDC_IF_ROUTER_TX_MUX_CFG0;
			shift = (tx_port << 1);
			shift_val = 0x03;
		} else if ((tx_port >= 4) && (tx_port < 8)) {
			tx_port_reg = WCD9335_CDC_IF_ROUTER_TX_MUX_CFG1;
			shift = ((tx_port - 4) << 1);
			shift_val = 0x03;
		} else if ((tx_port >= 8) && (tx_port < 11)) {
			tx_port_reg = WCD9335_CDC_IF_ROUTER_TX_MUX_CFG2;
			shift = ((tx_port - 8) << 1);
			shift_val = 0x03;
		} else if (tx_port == 11) {
			tx_port_reg = WCD9335_CDC_IF_ROUTER_TX_MUX_CFG3;
			shift = 0;
			shift_val = 0x0F;
		} else if (tx_port == 13) {
			tx_port_reg = WCD9335_CDC_IF_ROUTER_TX_MUX_CFG3;
			shift = 4;
			shift_val = 0x03;
		} else {
			return -EINVAL;
		}

		tx_mux_sel = snd_soc_component_read32(comp, tx_port_reg) &
						      (shift_val << shift);

		tx_mux_sel = tx_mux_sel >> shift;
		if (tx_port <= 8) {
			if ((tx_mux_sel == 0x2) || (tx_mux_sel == 0x3))
				decimator = tx_port;
		} else if (tx_port <= 10) {
			if ((tx_mux_sel == 0x1) || (tx_mux_sel == 0x2))
				decimator = ((tx_port == 9) ? 7 : 6);
		} else if (tx_port == 11) {
			if ((tx_mux_sel >= 1) && (tx_mux_sel < 7))
				decimator = tx_mux_sel - 1;
		} else if (tx_port == 13) {
			if ((tx_mux_sel == 0x1) || (tx_mux_sel == 0x2))
				decimator = 5;
		}

		if (decimator >= 0) {
			snd_soc_component_update_bits(comp,
					WCD9335_CDC_TX_PATH_CTL(decimator),
					WCD9335_CDC_TX_PATH_CTL_PCM_RATE_MASK,
					rate_val);
		} else if ((tx_port <= 8) && (tx_mux_sel == 0x01)) {
			/* Check if the TX Mux input is RX MIX TXn */
			dev_err(wcd->dev, "RX_MIX_TX%u going to SLIM TX%u\n",
				tx_port, tx_port);
		} else {
			dev_err(wcd->dev, "ERROR: Invalid decimator: %d\n",
				decimator);
			return -EINVAL;
		}
	}

	return 0;
}