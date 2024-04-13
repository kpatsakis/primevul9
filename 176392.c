static int vidioc_querycap(struct file *file, void *priv,
					struct v4l2_capability *v)
{
	struct raremono_device *radio = video_drvdata(file);

	strscpy(v->driver, "radio-raremono", sizeof(v->driver));
	strscpy(v->card, "Thanko's Raremono", sizeof(v->card));
	usb_make_path(radio->usbdev, v->bus_info, sizeof(v->bus_info));
	return 0;
}