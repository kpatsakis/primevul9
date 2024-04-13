static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct raremono_device *radio = video_drvdata(file);

	if (f->tuner != 0)
		return -EINVAL;
	f->type = V4L2_TUNER_RADIO;
	f->frequency = radio->curfreq * 16;
	return 0;
}