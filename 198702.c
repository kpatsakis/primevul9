static int tw5864_fmt_vid_cap(struct file *file, void *priv,
			      struct v4l2_format *f)
{
	struct tw5864_input *input = video_drvdata(file);

	f->fmt.pix.width = 704;
	switch (input->std) {
	default:
		WARN_ON_ONCE(1);
		return -EINVAL;
	case STD_NTSC:
		f->fmt.pix.height = 480;
		break;
	case STD_PAL:
	case STD_SECAM:
		f->fmt.pix.height = 576;
		break;
	}
	f->fmt.pix.field = V4L2_FIELD_INTERLACED;
	f->fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
	f->fmt.pix.sizeimage = H264_VLC_BUF_SIZE;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
	return 0;
}