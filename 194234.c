static int cpia2_mmap(struct file *file, struct vm_area_struct *area)
{
	struct camera_data *cam = video_drvdata(file);
	int retval;

	if (mutex_lock_interruptible(&cam->v4l2_lock))
		return -ERESTARTSYS;
	retval = cpia2_remap_buffer(cam, area);

	if(!retval)
		cam->stream_fh = file->private_data;
	mutex_unlock(&cam->v4l2_lock);
	return retval;
}