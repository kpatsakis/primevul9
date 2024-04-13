static int cpia2_enum_framesizes(struct file *file, void *fh,
					 struct v4l2_frmsizeenum *fsize)
{

	if (fsize->pixel_format != V4L2_PIX_FMT_MJPEG &&
	    fsize->pixel_format != V4L2_PIX_FMT_JPEG)
		return -EINVAL;
	if (fsize->index >= ARRAY_SIZE(cpia2_framesizes))
		return -EINVAL;
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->discrete.width = cpia2_framesizes[fsize->index].width;
	fsize->discrete.height = cpia2_framesizes[fsize->index].height;

	return 0;
}