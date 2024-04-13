static int cpia2_querycap(struct file *file, void *fh, struct v4l2_capability *vc)
{
	struct camera_data *cam = video_drvdata(file);

	strscpy(vc->driver, "cpia2", sizeof(vc->driver));

	if (cam->params.pnp_id.product == 0x151)
		strscpy(vc->card, "QX5 Microscope", sizeof(vc->card));
	else
		strscpy(vc->card, "CPiA2 Camera", sizeof(vc->card));
	switch (cam->params.pnp_id.device_type) {
	case DEVICE_STV_672:
		strcat(vc->card, " (672/");
		break;
	case DEVICE_STV_676:
		strcat(vc->card, " (676/");
		break;
	default:
		strcat(vc->card, " (XXX/");
		break;
	}
	switch (cam->params.version.sensor_flags) {
	case CPIA2_VP_SENSOR_FLAGS_404:
		strcat(vc->card, "404)");
		break;
	case CPIA2_VP_SENSOR_FLAGS_407:
		strcat(vc->card, "407)");
		break;
	case CPIA2_VP_SENSOR_FLAGS_409:
		strcat(vc->card, "409)");
		break;
	case CPIA2_VP_SENSOR_FLAGS_410:
		strcat(vc->card, "410)");
		break;
	case CPIA2_VP_SENSOR_FLAGS_500:
		strcat(vc->card, "500)");
		break;
	default:
		strcat(vc->card, "XXX)");
		break;
	}

	if (usb_make_path(cam->dev, vc->bus_info, sizeof(vc->bus_info)) <0)
		memset(vc->bus_info,0, sizeof(vc->bus_info));

	vc->device_caps = V4L2_CAP_VIDEO_CAPTURE |
			   V4L2_CAP_READWRITE |
			   V4L2_CAP_STREAMING;
	vc->capabilities = vc->device_caps |
			   V4L2_CAP_DEVICE_CAPS;

	return 0;
}