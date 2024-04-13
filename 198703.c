static int tw5864_enum_frameintervals(struct file *file, void *priv,
				      struct v4l2_frmivalenum *fintv)
{
	struct tw5864_input *input = video_drvdata(file);
	struct v4l2_fract frameinterval;
	int std_max_fps = input->std == STD_NTSC ? 30 : 25;
	struct v4l2_frmsizeenum fsize = { .index = fintv->index,
		.pixel_format = fintv->pixel_format };
	int ret;

	ret = tw5864_enum_framesizes(file, priv, &fsize);
	if (ret)
		return ret;

	if (fintv->width != fsize.discrete.width ||
	    fintv->height != fsize.discrete.height)
		return -EINVAL;

	fintv->type = V4L2_FRMIVAL_TYPE_STEPWISE;

	ret = tw5864_frameinterval_get(input, &frameinterval);
	fintv->stepwise.step = frameinterval;
	fintv->stepwise.min = frameinterval;
	fintv->stepwise.max = frameinterval;
	fintv->stepwise.max.numerator *= std_max_fps;

	return ret;
}