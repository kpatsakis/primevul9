static int cpia2_dqbuf(struct file *file, void *fh, struct v4l2_buffer *buf)
{
	struct camera_data *cam = video_drvdata(file);
	int frame;

	if(buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	   buf->memory != V4L2_MEMORY_MMAP)
		return -EINVAL;

	frame = find_earliest_filled_buffer(cam);

	if(frame < 0 && file->f_flags&O_NONBLOCK)
		return -EAGAIN;

	if(frame < 0) {
		/* Wait for a frame to become available */
		struct framebuf *cb=cam->curbuff;
		mutex_unlock(&cam->v4l2_lock);
		wait_event_interruptible(cam->wq_stream,
					 !video_is_registered(&cam->vdev) ||
					 (cb=cam->curbuff)->status == FRAME_READY);
		mutex_lock(&cam->v4l2_lock);
		if (signal_pending(current))
			return -ERESTARTSYS;
		if (!video_is_registered(&cam->vdev))
			return -ENOTTY;
		frame = cb->num;
	}


	buf->index = frame;
	buf->bytesused = cam->buffers[buf->index].length;
	buf->flags = V4L2_BUF_FLAG_MAPPED | V4L2_BUF_FLAG_DONE
		| V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	buf->field = V4L2_FIELD_NONE;
	buf->timestamp = ns_to_timeval(cam->buffers[buf->index].ts);
	buf->sequence = cam->buffers[buf->index].seq;
	buf->m.offset = cam->buffers[buf->index].data - cam->frame_buffer;
	buf->length = cam->frame_size;
	buf->reserved2 = 0;
	buf->request_fd = 0;
	memset(&buf->timecode, 0, sizeof(buf->timecode));

	DBG("DQBUF #%d status:%d seq:%d length:%d\n", buf->index,
	    cam->buffers[buf->index].status, buf->sequence, buf->bytesused);

	return 0;
}