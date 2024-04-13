static int wcd9335_set_interpolator_rate(struct snd_soc_dai *dai, u32 rate)
{
	int i;

	/* set mixing path rate */
	for (i = 0; i < ARRAY_SIZE(int_mix_rate_val); i++) {
		if (rate == int_mix_rate_val[i].rate) {
			wcd9335_set_mix_interpolator_rate(dai,
					int_mix_rate_val[i].rate_val, rate);
			break;
		}
	}

	/* set primary path sample rate */
	for (i = 0; i < ARRAY_SIZE(int_prim_rate_val); i++) {
		if (rate == int_prim_rate_val[i].rate) {
			wcd9335_set_prim_interpolator_rate(dai,
					int_prim_rate_val[i].rate_val, rate);
			break;
		}
	}

	return 0;
}