static int vidioc_enum_freq_bands(struct file *file, void *priv,
		struct v4l2_frequency_band *band)
{
	if (band->tuner != 0)
		return -EINVAL;

	if (band->index >= ARRAY_SIZE(bands))
		return -EINVAL;

	*band = bands[band->index];

	return 0;
}