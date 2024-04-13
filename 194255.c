static int cpia2_s_parm(struct file *file, void *fh, struct v4l2_streamparm *p)
{
	struct camera_data *cam = video_drvdata(file);
	struct v4l2_captureparm *cap = &p->parm.capture;
	struct v4l2_fract tpf = cap->timeperframe;
	int max = ARRAY_SIZE(framerate_controls) - 1;
	int ret;
	int i;

	ret = cpia2_g_parm(file, fh, p);
	if (ret || !tpf.denominator || !tpf.numerator)
		return ret;

	/* Maximum 15 fps for this model */
	if (cam->params.pnp_id.device_type == DEVICE_STV_672 &&
	    cam->params.version.sensor_flags == CPIA2_VP_SENSOR_FLAGS_500)
		max -= 2;
	for (i = 0; i <= max; i++) {
		struct v4l2_fract f1 = tpf;
		struct v4l2_fract f2 = framerate_controls[i].period;

		f1.numerator *= f2.denominator;
		f2.numerator *= f1.denominator;
		if (f1.numerator >= f2.numerator)
			break;
	}
	if (i > max)
		i = max;
	cap->timeperframe = framerate_controls[i].period;
	return cpia2_set_fps(cam, framerate_controls[i].value);
}