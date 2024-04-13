static int tw5864_s_parm(struct file *file, void *priv,
			 struct v4l2_streamparm *sp)
{
	struct tw5864_input *input = video_drvdata(file);
	struct v4l2_fract *t = &sp->parm.capture.timeperframe;
	struct v4l2_fract time_base;
	int ret;

	ret = tw5864_frameinterval_get(input, &time_base);
	if (ret)
		return ret;

	if (!t->numerator || !t->denominator) {
		t->numerator = time_base.numerator * input->frame_interval;
		t->denominator = time_base.denominator;
	} else if (t->denominator != time_base.denominator) {
		t->numerator = t->numerator * time_base.denominator /
			t->denominator;
		t->denominator = time_base.denominator;
	}

	input->frame_interval = t->numerator / time_base.numerator;
	if (input->frame_interval < 1)
		input->frame_interval = 1;
	tw5864_frame_interval_set(input);
	return tw5864_g_parm(file, priv, sp);
}