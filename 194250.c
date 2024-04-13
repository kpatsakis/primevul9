static int cpia2_try_fmt_vid_cap(struct file *file, void *fh,
					  struct v4l2_format *f)
{
	struct camera_data *cam = video_drvdata(file);

	if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_MJPEG &&
	    f->fmt.pix.pixelformat != V4L2_PIX_FMT_JPEG)
	       return -EINVAL;

	f->fmt.pix.field = V4L2_FIELD_NONE;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage = cam->frame_size;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
	f->fmt.pix.priv = 0;

	switch (cpia2_match_video_size(f->fmt.pix.width, f->fmt.pix.height)) {
	case VIDEOSIZE_VGA:
		f->fmt.pix.width = 640;
		f->fmt.pix.height = 480;
		break;
	case VIDEOSIZE_CIF:
		f->fmt.pix.width = 352;
		f->fmt.pix.height = 288;
		break;
	case VIDEOSIZE_QVGA:
		f->fmt.pix.width = 320;
		f->fmt.pix.height = 240;
		break;
	case VIDEOSIZE_288_216:
		f->fmt.pix.width = 288;
		f->fmt.pix.height = 216;
		break;
	case VIDEOSIZE_256_192:
		f->fmt.pix.width = 256;
		f->fmt.pix.height = 192;
		break;
	case VIDEOSIZE_224_168:
		f->fmt.pix.width = 224;
		f->fmt.pix.height = 168;
		break;
	case VIDEOSIZE_192_144:
		f->fmt.pix.width = 192;
		f->fmt.pix.height = 144;
		break;
	case VIDEOSIZE_QCIF:
	default:
		f->fmt.pix.width = 176;
		f->fmt.pix.height = 144;
		break;
	}

	return 0;
}