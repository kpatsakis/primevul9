static int cpia2_s_jpegcomp(struct file *file, void *fh,
		const struct v4l2_jpegcompression *parms)
{
	struct camera_data *cam = video_drvdata(file);

	DBG("S_JPEGCOMP APP_len:%d COM_len:%d\n",
	    parms->APP_len, parms->COM_len);

	cam->params.compression.inhibit_htables =
		!(parms->jpeg_markers & V4L2_JPEG_MARKER_DHT);

	if(parms->APP_len != 0) {
		if(parms->APP_len > 0 &&
		   parms->APP_len <= sizeof(cam->APP_data) &&
		   parms->APPn >= 0 && parms->APPn <= 15) {
			cam->APPn = parms->APPn;
			cam->APP_len = parms->APP_len;
			memcpy(cam->APP_data, parms->APP_data, parms->APP_len);
		} else {
			LOG("Bad APPn Params n=%d len=%d\n",
			    parms->APPn, parms->APP_len);
			return -EINVAL;
		}
	} else {
		cam->APP_len = 0;
	}

	if(parms->COM_len != 0) {
		if(parms->COM_len > 0 &&
		   parms->COM_len <= sizeof(cam->COM_data)) {
			cam->COM_len = parms->COM_len;
			memcpy(cam->COM_data, parms->COM_data, parms->COM_len);
		} else {
			LOG("Bad COM_len=%d\n", parms->COM_len);
			return -EINVAL;
		}
	}

	return 0;
}