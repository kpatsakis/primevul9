static int vidioc_s_frequency(struct file *file, void *priv,
				const struct v4l2_frequency *f)
{
	struct raremono_device *radio = video_drvdata(file);
	u32 freq;
	unsigned band;

	if (f->tuner != 0 || f->type != V4L2_TUNER_RADIO)
		return -EINVAL;

	if (f->frequency >= (FM_FREQ_RANGE_LOW + SW_FREQ_RANGE_HIGH) * 8)
		band = BAND_FM;
	else if (f->frequency <= (AM_FREQ_RANGE_HIGH + SW_FREQ_RANGE_LOW) * 8)
		band = BAND_AM;
	else
		band = BAND_SW;

	freq = clamp_t(u32, f->frequency, bands[band].rangelow, bands[band].rangehigh);
	return raremono_cmd_main(radio, band, freq / 16);
}