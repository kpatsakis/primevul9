static int tw5864_is_motion_triggered(struct tw5864_h264_frame *frame)
{
	struct tw5864_input *input = frame->input;
	u32 *mv = (u32 *)frame->mv.addr;
	int i;
	int detected = 0;

	for (i = 0; i < MD_CELLS; i++) {
		const u16 thresh = input->md_threshold_grid_values[i];
		const unsigned int metric = tw5864_md_metric_from_mvd(mv[i]);

		if (metric > thresh)
			detected = 1;

		if (detected)
			break;
	}
	return detected;
}