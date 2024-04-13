static ssize_t cpia2_v4l_read(struct file *file, char __user *buf, size_t count,
			      loff_t *off)
{
	struct camera_data *cam = video_drvdata(file);
	int noblock = file->f_flags&O_NONBLOCK;
	ssize_t ret;

	if(!cam)
		return -EINVAL;

	if (mutex_lock_interruptible(&cam->v4l2_lock))
		return -ERESTARTSYS;
	ret = cpia2_read(cam, buf, count, noblock);
	mutex_unlock(&cam->v4l2_lock);
	return ret;
}