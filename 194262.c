static int cpia2_streamoff(struct file *file, void *fh, enum v4l2_buf_type type)
{
	struct camera_data *cam = video_drvdata(file);
	int ret = -EINVAL;

	DBG("VIDIOC_STREAMOFF, streaming=%d\n", cam->streaming);
	if (!cam->mmapped || type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if (cam->streaming) {
		ret = cpia2_usb_stream_stop(cam);
		if (!ret)
			v4l2_ctrl_grab(cam->usb_alt, false);
	}
	return ret;
}