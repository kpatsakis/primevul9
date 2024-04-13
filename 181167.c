static inline int ad5755_get_offset(struct ad5755_state *st,
	struct iio_chan_spec const *chan)
{
	int min, max;

	ad5755_get_min_max(st, chan, &min, &max);
	return (min * (1 << chan->scan_type.realbits)) / (max - min);
}