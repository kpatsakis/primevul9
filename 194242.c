static int cpia2_enum_fmt_vid_cap(struct file *file, void *fh,
					    struct v4l2_fmtdesc *f)
{
	int index = f->index;

	if (index < 0 || index > 1)
	       return -EINVAL;

	memset(f, 0, sizeof(*f));
	f->index = index;
	f->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	f->flags = V4L2_FMT_FLAG_COMPRESSED;
	switch(index) {
	case 0:
		strscpy(f->description, "MJPEG", sizeof(f->description));
		f->pixelformat = V4L2_PIX_FMT_MJPEG;
		break;
	case 1:
		strscpy(f->description, "JPEG", sizeof(f->description));
		f->pixelformat = V4L2_PIX_FMT_JPEG;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}