static void reset_camera_struct_v4l(struct camera_data *cam)
{
	cam->width = cam->params.roi.width;
	cam->height = cam->params.roi.height;

	cam->frame_size = buffer_size;
	cam->num_frames = num_buffers;

	/* Flicker modes */
	cam->params.flicker_control.flicker_mode_req = flicker_mode;

	/* stream modes */
	cam->params.camera_state.stream_mode = alternate;

	cam->pixelformat = V4L2_PIX_FMT_JPEG;
}