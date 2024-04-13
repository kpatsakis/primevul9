static int tw5864_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct tw5864_input *input =
		container_of(ctrl->handler, struct tw5864_input, hdl);
	struct tw5864_dev *dev = input->root;
	unsigned long flags;

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		tw_indir_writeb(TW5864_INDIR_VIN_A_BRIGHT(input->nr),
				(u8)ctrl->val);
		break;
	case V4L2_CID_HUE:
		tw_indir_writeb(TW5864_INDIR_VIN_7_HUE(input->nr),
				(u8)ctrl->val);
		break;
	case V4L2_CID_CONTRAST:
		tw_indir_writeb(TW5864_INDIR_VIN_9_CNTRST(input->nr),
				(u8)ctrl->val);
		break;
	case V4L2_CID_SATURATION:
		tw_indir_writeb(TW5864_INDIR_VIN_B_SAT_U(input->nr),
				(u8)ctrl->val);
		tw_indir_writeb(TW5864_INDIR_VIN_C_SAT_V(input->nr),
				(u8)ctrl->val);
		break;
	case V4L2_CID_MPEG_VIDEO_GOP_SIZE:
		input->gop = ctrl->val;
		return 0;
	case V4L2_CID_MPEG_VIDEO_H264_MIN_QP:
		spin_lock_irqsave(&input->slock, flags);
		input->qp = ctrl->val;
		input->reg_dsp_qp = input->qp;
		input->reg_dsp_ref_mvp_lambda = lambda_lookup_table[input->qp];
		input->reg_dsp_i4x4_weight = intra4x4_lambda3[input->qp];
		spin_unlock_irqrestore(&input->slock, flags);
		return 0;
	case V4L2_CID_DETECT_MD_GLOBAL_THRESHOLD:
		memset(input->md_threshold_grid_values, ctrl->val,
		       sizeof(input->md_threshold_grid_values));
		return 0;
	case V4L2_CID_DETECT_MD_MODE:
		return 0;
	case V4L2_CID_DETECT_MD_THRESHOLD_GRID:
		/* input->md_threshold_grid_ctrl->p_new.p_u16 contains data */
		memcpy(input->md_threshold_grid_values,
		       input->md_threshold_grid_ctrl->p_new.p_u16,
		       sizeof(input->md_threshold_grid_values));
		return 0;
	}
	return 0;
}