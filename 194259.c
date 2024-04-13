static int cpia2_streamon(struct file *file, void *fh, enum v4l2_buf_type type)
{
	struct camera_data *cam = video_drvdata(file);
	int ret = -EINVAL;

	DBG("VIDIOC_STREAMON, streaming=%d\n", cam->streaming);
	if (!cam->mmapped || type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if (!cam->streaming) {
		ret = cpia2_usb_stream_start(cam,
				cam->params.camera_state.stream_mode);
		if (!ret)
			v4l2_ctrl_grab(cam->usb_alt, true);
	}
	return ret;
}