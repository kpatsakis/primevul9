static int cpia2_close(struct file *file)
{
	struct video_device *dev = video_devdata(file);
	struct camera_data *cam = video_get_drvdata(dev);

	mutex_lock(&cam->v4l2_lock);
	if (video_is_registered(&cam->vdev) && v4l2_fh_is_singular_file(file)) {
		cpia2_usb_stream_stop(cam);

		/* save camera state for later open */
		cpia2_save_camera_state(cam);

		cpia2_set_low_power(cam);
		cpia2_free_buffers(cam);
	}

	if (cam->stream_fh == file->private_data) {
		cam->stream_fh = NULL;
		cam->mmapped = 0;
	}
	mutex_unlock(&cam->v4l2_lock);
	return v4l2_fh_release(file);
}