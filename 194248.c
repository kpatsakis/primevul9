static int cpia2_enum_frameintervals(struct file *file, void *fh,
					   struct v4l2_frmivalenum *fival)
{
	struct camera_data *cam = video_drvdata(file);
	int max = ARRAY_SIZE(framerate_controls) - 1;
	int i;

	if (fival->pixel_format != V4L2_PIX_FMT_MJPEG &&
	    fival->pixel_format != V4L2_PIX_FMT_JPEG)
		return -EINVAL;

	/* Maximum 15 fps for this model */
	if (cam->params.pnp_id.device_type == DEVICE_STV_672 &&
	    cam->params.version.sensor_flags == CPIA2_VP_SENSOR_FLAGS_500)
		max -= 2;
	if (fival->index > max)
		return -EINVAL;
	for (i = 0; i < ARRAY_SIZE(cpia2_framesizes); i++)
		if (fival->width == cpia2_framesizes[i].width &&
		    fival->height == cpia2_framesizes[i].height)
			break;
	if (i == ARRAY_SIZE(cpia2_framesizes))
		return -EINVAL;
	fival->type = V4L2_FRMIVAL_TYPE_DISCRETE;
	fival->discrete = framerate_controls[fival->index].period;
	return 0;
}