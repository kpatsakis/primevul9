static v4l2_std_id tw5864_get_v4l2_std(enum tw5864_vid_std std)
{
	switch (std) {
	case STD_NTSC:    return V4L2_STD_NTSC_M;
	case STD_PAL:     return V4L2_STD_PAL_B;
	case STD_SECAM:   return V4L2_STD_SECAM_B;
	case STD_NTSC443: return V4L2_STD_NTSC_443;
	case STD_PAL_M:   return V4L2_STD_PAL_M;
	case STD_PAL_CN:  return V4L2_STD_PAL_Nc;
	case STD_PAL_60:  return V4L2_STD_PAL_60;
	case STD_INVALID: return V4L2_STD_UNKNOWN;
	}
	return 0;
}