int cpia2_register_camera(struct camera_data *cam)
{
	struct v4l2_ctrl_handler *hdl = &cam->hdl;
	struct v4l2_ctrl_config cpia2_usb_alt = {
		.ops = &cpia2_ctrl_ops,
		.id = CPIA2_CID_USB_ALT,
		.name = "USB Alternate",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = USBIF_ISO_1,
		.max = USBIF_ISO_6,
		.step = 1,
	};
	int ret;

	v4l2_ctrl_handler_init(hdl, 12);
	v4l2_ctrl_new_std(hdl, &cpia2_ctrl_ops,
			V4L2_CID_BRIGHTNESS,
			cam->params.pnp_id.device_type == DEVICE_STV_672 ? 1 : 0,
			255, 1, DEFAULT_BRIGHTNESS);
	v4l2_ctrl_new_std(hdl, &cpia2_ctrl_ops,
			V4L2_CID_CONTRAST, 0, 255, 1, DEFAULT_CONTRAST);
	v4l2_ctrl_new_std(hdl, &cpia2_ctrl_ops,
			V4L2_CID_SATURATION, 0, 255, 1, DEFAULT_SATURATION);
	v4l2_ctrl_new_std(hdl, &cpia2_ctrl_ops,
			V4L2_CID_HFLIP, 0, 1, 1, 0);
	v4l2_ctrl_new_std(hdl, &cpia2_ctrl_ops,
			V4L2_CID_JPEG_ACTIVE_MARKER, 0,
			V4L2_JPEG_ACTIVE_MARKER_DHT, 0,
			V4L2_JPEG_ACTIVE_MARKER_DHT);
	v4l2_ctrl_new_std(hdl, &cpia2_ctrl_ops,
			V4L2_CID_JPEG_COMPRESSION_QUALITY, 1,
			100, 1, 100);
	cpia2_usb_alt.def = alternate;
	cam->usb_alt = v4l2_ctrl_new_custom(hdl, &cpia2_usb_alt, NULL);
	/* VP5 Only */
	if (cam->params.pnp_id.device_type != DEVICE_STV_672)
		v4l2_ctrl_new_std(hdl, &cpia2_ctrl_ops,
			V4L2_CID_VFLIP, 0, 1, 1, 0);
	/* Flicker control only valid for 672 */
	if (cam->params.pnp_id.device_type == DEVICE_STV_672)
		v4l2_ctrl_new_std_menu(hdl, &cpia2_ctrl_ops,
			V4L2_CID_POWER_LINE_FREQUENCY,
			V4L2_CID_POWER_LINE_FREQUENCY_60HZ, 0, 0);
	/* Light control only valid for the QX5 Microscope */
	if (cam->params.pnp_id.product == 0x151) {
		cam->top_light = v4l2_ctrl_new_std(hdl, &cpia2_ctrl_ops,
				V4L2_CID_ILLUMINATORS_1, 0, 1, 1, 0);
		cam->bottom_light = v4l2_ctrl_new_std(hdl, &cpia2_ctrl_ops,
				V4L2_CID_ILLUMINATORS_2, 0, 1, 1, 0);
		v4l2_ctrl_cluster(2, &cam->top_light);
	}

	if (hdl->error) {
		ret = hdl->error;
		v4l2_ctrl_handler_free(hdl);
		return ret;
	}

	cam->vdev = cpia2_template;
	video_set_drvdata(&cam->vdev, cam);
	cam->vdev.lock = &cam->v4l2_lock;
	cam->vdev.ctrl_handler = hdl;
	cam->vdev.v4l2_dev = &cam->v4l2_dev;

	reset_camera_struct_v4l(cam);

	/* register v4l device */
	if (video_register_device(&cam->vdev, VFL_TYPE_GRABBER, video_nr) < 0) {
		ERR("video_register_device failed\n");
		return -ENODEV;
	}

	return 0;
}