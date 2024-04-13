static int cpia2_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct camera_data *cam =
		container_of(ctrl->handler, struct camera_data, hdl);
	static const int flicker_table[] = {
		NEVER_FLICKER,
		FLICKER_50,
		FLICKER_60,
	};

	DBG("Set control id:%d, value:%d\n", ctrl->id, ctrl->val);

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		cpia2_set_brightness(cam, ctrl->val);
		break;
	case V4L2_CID_CONTRAST:
		cpia2_set_contrast(cam, ctrl->val);
		break;
	case V4L2_CID_SATURATION:
		cpia2_set_saturation(cam, ctrl->val);
		break;
	case V4L2_CID_HFLIP:
		cpia2_set_property_mirror(cam, ctrl->val);
		break;
	case V4L2_CID_VFLIP:
		cpia2_set_property_flip(cam, ctrl->val);
		break;
	case V4L2_CID_POWER_LINE_FREQUENCY:
		return cpia2_set_flicker_mode(cam, flicker_table[ctrl->val]);
	case V4L2_CID_ILLUMINATORS_1:
		return cpia2_set_gpio(cam, (cam->top_light->val << 6) |
					   (cam->bottom_light->val << 7));
	case V4L2_CID_JPEG_ACTIVE_MARKER:
		cam->params.compression.inhibit_htables =
			!(ctrl->val & V4L2_JPEG_ACTIVE_MARKER_DHT);
		break;
	case V4L2_CID_JPEG_COMPRESSION_QUALITY:
		cam->params.vc_params.quality = ctrl->val;
		break;
	case CPIA2_CID_USB_ALT:
		cam->params.camera_state.stream_mode = ctrl->val;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}