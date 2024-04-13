static int vidioc_g_tuner(struct file *file, void *priv,
		struct v4l2_tuner *v)
{
	struct raremono_device *radio = video_drvdata(file);
	int ret;

	if (v->index > 0)
		return -EINVAL;

	strscpy(v->name, "AM/FM/SW", sizeof(v->name));
	v->capability = V4L2_TUNER_CAP_LOW | V4L2_TUNER_CAP_STEREO |
		V4L2_TUNER_CAP_FREQ_BANDS;
	v->rangelow = AM_FREQ_RANGE_LOW * 16;
	v->rangehigh = FM_FREQ_RANGE_HIGH * 16;
	v->rxsubchans = V4L2_TUNER_SUB_STEREO | V4L2_TUNER_SUB_MONO;
	v->audmode = (radio->curfreq < FM_FREQ_RANGE_LOW) ?
		V4L2_TUNER_MODE_MONO : V4L2_TUNER_MODE_STEREO;
	memset(radio->buffer, 1, BUFFER_LENGTH);
	ret = usb_control_msg(radio->usbdev, usb_rcvctrlpipe(radio->usbdev, 0),
			1, 0xa1, 0x030d, 2, radio->buffer, BUFFER_LENGTH, USB_TIMEOUT);

	if (ret < 0) {
		dev_warn(radio->v4l2_dev.dev, "%s failed (%d)\n", __func__, ret);
		return ret;
	}
	v->signal = ((radio->buffer[1] & 0xf) << 8 | radio->buffer[2]) << 4;
	return 0;
}