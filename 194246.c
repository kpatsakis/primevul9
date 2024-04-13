void cpia2_camera_release(struct v4l2_device *v4l2_dev)
{
	struct camera_data *cam =
		container_of(v4l2_dev, struct camera_data, v4l2_dev);

	v4l2_ctrl_handler_free(&cam->hdl);
	v4l2_device_unregister(&cam->v4l2_dev);
	kfree(cam);
}