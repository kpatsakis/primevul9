static int cpia2_open(struct file *file)
{
	struct camera_data *cam = video_drvdata(file);
	int retval;

	if (mutex_lock_interruptible(&cam->v4l2_lock))
		return -ERESTARTSYS;
	retval = v4l2_fh_open(file);
	if (retval)
		goto open_unlock;

	if (v4l2_fh_is_singular_file(file)) {
		if (cpia2_allocate_buffers(cam)) {
			v4l2_fh_release(file);
			retval = -ENOMEM;
			goto open_unlock;
		}

		/* reset the camera */
		if (cpia2_reset_camera(cam) < 0) {
			v4l2_fh_release(file);
			retval = -EIO;
			goto open_unlock;
		}

		cam->APP_len = 0;
		cam->COM_len = 0;
	}

	cpia2_dbg_dump_registers(cam);
open_unlock:
	mutex_unlock(&cam->v4l2_lock);
	return retval;
}