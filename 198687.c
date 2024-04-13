static int tw5864_enum_framesizes(struct file *file, void *priv,
				  struct v4l2_frmsizeenum *fsize)
{
	struct tw5864_input *input = video_drvdata(file);

	if (fsize->index > 0)
		return -EINVAL;
	if (fsize->pixel_format != V4L2_PIX_FMT_H264)
		return -EINVAL;

	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->discrete.width = 704;
	fsize->discrete.height = input->std == STD_NTSC ? 480 : 576;

	return 0;
}