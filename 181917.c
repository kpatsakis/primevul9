static int wcd9335_slim_set_hw_params(struct wcd9335_codec *wcd,
				 struct wcd_slim_codec_dai_data *dai_data,
				 int direction)
{
	struct list_head *slim_ch_list = &dai_data->slim_ch_list;
	struct slim_stream_config *cfg = &dai_data->sconfig;
	struct wcd9335_slim_ch *ch;
	u16 payload = 0;
	int ret, i;

	cfg->ch_count = 0;
	cfg->direction = direction;
	cfg->port_mask = 0;

	/* Configure slave interface device */
	list_for_each_entry(ch, slim_ch_list, list) {
		cfg->ch_count++;
		payload |= 1 << ch->shift;
		cfg->port_mask |= BIT(ch->port);
	}

	cfg->chs = kcalloc(cfg->ch_count, sizeof(unsigned int), GFP_KERNEL);
	if (!cfg->chs)
		return -ENOMEM;

	i = 0;
	list_for_each_entry(ch, slim_ch_list, list) {
		cfg->chs[i++] = ch->ch_num;
		if (direction == SNDRV_PCM_STREAM_PLAYBACK) {
			/* write to interface device */
			ret = regmap_write(wcd->if_regmap,
				WCD9335_SLIM_PGD_RX_PORT_MULTI_CHNL_0(ch->port),
				payload);

			if (ret < 0)
				goto err;

			/* configure the slave port for water mark and enable*/
			ret = regmap_write(wcd->if_regmap,
					WCD9335_SLIM_PGD_RX_PORT_CFG(ch->port),
					WCD9335_SLIM_WATER_MARK_VAL);
			if (ret < 0)
				goto err;
		} else {
			ret = regmap_write(wcd->if_regmap,
				WCD9335_SLIM_PGD_TX_PORT_MULTI_CHNL_0(ch->port),
				payload & 0x00FF);
			if (ret < 0)
				goto err;

			/* ports 8,9 */
			ret = regmap_write(wcd->if_regmap,
				WCD9335_SLIM_PGD_TX_PORT_MULTI_CHNL_1(ch->port),
				(payload & 0xFF00)>>8);
			if (ret < 0)
				goto err;

			/* configure the slave port for water mark and enable*/
			ret = regmap_write(wcd->if_regmap,
					WCD9335_SLIM_PGD_TX_PORT_CFG(ch->port),
					WCD9335_SLIM_WATER_MARK_VAL);

			if (ret < 0)
				goto err;
		}
	}

	dai_data->sruntime = slim_stream_allocate(wcd->slim, "WCD9335-SLIM");

	return 0;

err:
	dev_err(wcd->dev, "Error Setting slim hw params\n");
	kfree(cfg->chs);
	cfg->chs = NULL;

	return ret;
}