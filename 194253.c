static int cpia2_g_parm(struct file *file, void *fh, struct v4l2_streamparm *p)
{
	struct camera_data *cam = video_drvdata(file);
	struct v4l2_captureparm *cap = &p->parm.capture;
	int i;

	if (p->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	cap->capability = V4L2_CAP_TIMEPERFRAME;
	cap->readbuffers = cam->num_frames;
	for (i = 0; i < ARRAY_SIZE(framerate_controls); i++)
		if (cam->params.vp_params.frame_rate == framerate_controls[i].value) {
			cap->timeperframe = framerate_controls[i].period;
			break;
		}
	return 0;
}