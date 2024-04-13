static int cpia2_g_jpegcomp(struct file *file, void *fh, struct v4l2_jpegcompression *parms)
{
	struct camera_data *cam = video_drvdata(file);

	memset(parms, 0, sizeof(*parms));

	parms->quality = 80; // TODO: Can this be made meaningful?

	parms->jpeg_markers = V4L2_JPEG_MARKER_DQT | V4L2_JPEG_MARKER_DRI;
	if(!cam->params.compression.inhibit_htables) {
		parms->jpeg_markers |= V4L2_JPEG_MARKER_DHT;
	}

	parms->APPn = cam->APPn;
	parms->APP_len = cam->APP_len;
	if(cam->APP_len > 0) {
		memcpy(parms->APP_data, cam->APP_data, cam->APP_len);
		parms->jpeg_markers |= V4L2_JPEG_MARKER_APP;
	}

	parms->COM_len = cam->COM_len;
	if(cam->COM_len > 0) {
		memcpy(parms->COM_data, cam->COM_data, cam->COM_len);
		parms->jpeg_markers |= JPEG_MARKER_COM;
	}

	DBG("G_JPEGCOMP APP_len:%d COM_len:%d\n",
	    parms->APP_len, parms->COM_len);

	return 0;
}