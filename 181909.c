static void wcd9335_codec_enable_int_port(struct wcd_slim_codec_dai_data *dai,
					struct snd_soc_component *component)
{
	int port_num = 0;
	unsigned short reg = 0;
	unsigned int val = 0;
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);
	struct wcd9335_slim_ch *ch;

	list_for_each_entry(ch, &dai->slim_ch_list, list) {
		if (ch->port >= WCD9335_RX_START) {
			port_num = ch->port - WCD9335_RX_START;
			reg = WCD9335_SLIM_PGD_PORT_INT_EN0 + (port_num / 8);
		} else {
			port_num = ch->port;
			reg = WCD9335_SLIM_PGD_PORT_INT_TX_EN0 + (port_num / 8);
		}

		regmap_read(wcd->if_regmap, reg, &val);
		if (!(val & BIT(port_num % 8)))
			regmap_write(wcd->if_regmap, reg,
					val | BIT(port_num % 8));
	}
}