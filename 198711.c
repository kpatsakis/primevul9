static int tw5864_subscribe_event(struct v4l2_fh *fh,
				  const struct v4l2_event_subscription *sub)
{
	switch (sub->type) {
	case V4L2_EVENT_MOTION_DET:
		/*
		 * Allow for up to 30 events (1 second for NTSC) to be stored.
		 */
		return v4l2_event_subscribe(fh, sub, 30, NULL);
	default:
		return v4l2_ctrl_subscribe_event(fh, sub);
	}
}