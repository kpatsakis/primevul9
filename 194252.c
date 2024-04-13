static int cpia2_reqbufs(struct file *file, void *fh, struct v4l2_requestbuffers *req)
{
	struct camera_data *cam = video_drvdata(file);

	if(req->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
	   req->memory != V4L2_MEMORY_MMAP)
		return -EINVAL;

	DBG("REQBUFS requested:%d returning:%d\n", req->count, cam->num_frames);
	req->count = cam->num_frames;
	memset(&req->reserved, 0, sizeof(req->reserved));

	return 0;
}