static int sync(struct camera_data *cam, int frame_nr)
{
	struct framebuf *frame = &cam->buffers[frame_nr];

	while (1) {
		if (frame->status == FRAME_READY)
			return 0;

		if (!cam->streaming) {
			frame->status = FRAME_READY;
			frame->length = 0;
			return 0;
		}

		mutex_unlock(&cam->v4l2_lock);
		wait_event_interruptible(cam->wq_stream,
					 !cam->streaming ||
					 frame->status == FRAME_READY);
		mutex_lock(&cam->v4l2_lock);
		if (signal_pending(current))
			return -ERESTARTSYS;
		if (!video_is_registered(&cam->vdev))
			return -ENOTTY;
	}
}