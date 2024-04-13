static void tw5864_video_input_fini(struct tw5864_input *dev)
{
	video_unregister_device(&dev->vdev);
	v4l2_ctrl_handler_free(&dev->hdl);
	vb2_queue_release(&dev->vidq);
}