static int wcd9335_set_mix_interpolator_rate(struct snd_soc_dai *dai,
					     int rate_val,
					     u32 rate)
{
	struct snd_soc_component *component = dai->component;
	struct wcd9335_codec *wcd = dev_get_drvdata(component->dev);
	struct wcd9335_slim_ch *ch;
	int val, j;

	list_for_each_entry(ch, &wcd->dai[dai->id].slim_ch_list, list) {
		for (j = 0; j < WCD9335_NUM_INTERPOLATORS; j++) {
			val = snd_soc_component_read32(component,
					WCD9335_CDC_RX_INP_MUX_RX_INT_CFG1(j)) &
					WCD9335_CDC_RX_INP_MUX_RX_INT_SEL_MASK;

			if (val == (ch->shift + INTn_2_INP_SEL_RX0))
				snd_soc_component_update_bits(component,
						WCD9335_CDC_RX_PATH_MIX_CTL(j),
						WCD9335_CDC_MIX_PCM_RATE_MASK,
						rate_val);
		}
	}

	return 0;
}