static int cpia2_qbuf(struct file *file, void *fh, struct v4l2_buffer *buf)
{
	struct camera_data *cam = video_drvdata(file);

	if(buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	   buf->memory != V4L2_MEMORY_MMAP ||
	   buf->index >= cam->num_frames)
		return -EINVAL;

	DBG("QBUF #%d\n", buf->index);

	if(cam->buffers[buf->index].status == FRAME_READY)
		cam->buffers[buf->index].status = FRAME_EMPTY;

	return 0;
}