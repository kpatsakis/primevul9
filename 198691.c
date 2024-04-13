static int tw5864_g_parm(struct file *file, void *priv,
			 struct v4l2_streamparm *sp)
{
	struct tw5864_input *input = video_drvdata(file);
	struct v4l2_captureparm *cp = &sp->parm.capture;
	int ret;

	cp->capability = V4L2_CAP_TIMEPERFRAME;

	ret = tw5864_frameinterval_get(input, &cp->timeperframe);
	cp->timeperframe.numerator *= input->frame_interval;
	cp->capturemode = 0;
	cp->readbuffers = 2;

	return ret;
}