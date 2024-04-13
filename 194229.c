static __poll_t cpia2_v4l_poll(struct file *filp, struct poll_table_struct *wait)
{
	struct camera_data *cam = video_drvdata(filp);
	__poll_t res;

	mutex_lock(&cam->v4l2_lock);
	res = cpia2_poll(cam, filp, wait);
	mutex_unlock(&cam->v4l2_lock);
	return res;
}