void cpia2_unregister_camera(struct camera_data *cam)
{
	video_unregister_device(&cam->vdev);
}