static int cpia2_g_fmt_vid_cap(struct file *file, void *fh,
					struct v4l2_format *f)
{
	struct camera_data *cam = video_drvdata(file);

	f->fmt.pix.width = cam->width;
	f->fmt.pix.height = cam->height;
	f->fmt.pix.pixelformat = cam->pixelformat;
	f->fmt.pix.field = V4L2_FIELD_NONE;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage = cam->frame_size;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
	f->fmt.pix.priv = 0;

	return 0;
}