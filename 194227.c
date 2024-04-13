static int cpia2_querybuf(struct file *file, void *fh, struct v4l2_buffer *buf)
{
	struct camera_data *cam = video_drvdata(file);

	if(buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	   buf->index >= cam->num_frames)
		return -EINVAL;

	buf->m.offset = cam->buffers[buf->index].data - cam->frame_buffer;
	buf->length = cam->frame_size;

	buf->memory = V4L2_MEMORY_MMAP;

	if(cam->mmapped)
		buf->flags = V4L2_BUF_FLAG_MAPPED;
	else
		buf->flags = 0;

	buf->flags |= V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;

	switch (cam->buffers[buf->index].status) {
	case FRAME_EMPTY:
	case FRAME_ERROR:
	case FRAME_READING:
		buf->bytesused = 0;
		buf->flags = V4L2_BUF_FLAG_QUEUED;
		break;
	case FRAME_READY:
		buf->bytesused = cam->buffers[buf->index].length;
		buf->timestamp = ns_to_timeval(cam->buffers[buf->index].ts);
		buf->sequence = cam->buffers[buf->index].seq;
		buf->flags = V4L2_BUF_FLAG_DONE;
		break;
	}

	DBG("QUERYBUF index:%d offset:%d flags:%d seq:%d bytesused:%d\n",
	     buf->index, buf->m.offset, buf->flags, buf->sequence,
	     buf->bytesused);

	return 0;
}