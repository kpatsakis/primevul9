static int wcd9335_set_prim_interpolator_rate(struct snd_soc_dai *dai,
					      u8 rate_val,
					      u32 rate)
{
	struct snd_soc_component *comp = dai->component;
	struct wcd9335_codec *wcd = dev_get_drvdata(comp->dev);
	struct wcd9335_slim_ch *ch;
	u8 cfg0, cfg1, inp0_sel, inp1_sel, inp2_sel;
	int inp, j;

	list_for_each_entry(ch, &wcd->dai[dai->id].slim_ch_list, list) {
		inp = ch->shift + INTn_1_MIX_INP_SEL_RX0;
		/*
		 * Loop through all interpolator MUX inputs and find out
		 * to which interpolator input, the slim rx port
		 * is connected
		 */
		for (j = 0; j < WCD9335_NUM_INTERPOLATORS; j++) {
			cfg0 = snd_soc_component_read32(comp,
					WCD9335_CDC_RX_INP_MUX_RX_INT_CFG0(j));
			cfg1 = snd_soc_component_read32(comp,
					WCD9335_CDC_RX_INP_MUX_RX_INT_CFG1(j));

			inp0_sel = cfg0 &
				 WCD9335_CDC_RX_INP_MUX_RX_INT_SEL_MASK;
			inp1_sel = (cfg0 >> 4) &
				 WCD9335_CDC_RX_INP_MUX_RX_INT_SEL_MASK;
			inp2_sel = (cfg1 >> 4) &
				 WCD9335_CDC_RX_INP_MUX_RX_INT_SEL_MASK;

			if ((inp0_sel == inp) ||  (inp1_sel == inp) ||
			    (inp2_sel == inp)) {
				/* rate is in Hz */
				if ((j == 0) && (rate == 44100))
					dev_info(wcd->dev,
						"Cannot set 44.1KHz on INT0\n");
				else
					snd_soc_component_update_bits(comp,
						WCD9335_CDC_RX_PATH_CTL(j),
						WCD9335_CDC_MIX_PCM_RATE_MASK,
						rate_val);
			}
		}
	}

	return 0;
}