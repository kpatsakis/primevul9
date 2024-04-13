static int cpia2_g_selection(struct file *file, void *fh,
			     struct v4l2_selection *s)
{
	struct camera_data *cam = video_drvdata(file);

	if (s->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	switch (s->target) {
	case V4L2_SEL_TGT_CROP_BOUNDS:
	case V4L2_SEL_TGT_CROP_DEFAULT:
		s->r.left = 0;
		s->r.top = 0;
		s->r.width = cam->width;
		s->r.height = cam->height;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}