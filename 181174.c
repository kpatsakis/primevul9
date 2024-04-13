static void ad5755_get_min_max(struct ad5755_state *st,
	struct iio_chan_spec const *chan, int *min, int *max)
{
	enum ad5755_mode mode = st->ctrl[chan->channel] & 7;
	*min = ad5755_min_max_table[mode][0];
	*max = ad5755_min_max_table[mode][1];
}