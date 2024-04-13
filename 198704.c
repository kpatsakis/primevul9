static int tw5864_s_reg(struct file *file, void *fh,
			const struct v4l2_dbg_register *reg)
{
	struct tw5864_input *input = video_drvdata(file);
	struct tw5864_dev *dev = input->root;

	if (reg->reg < INDIR_SPACE_MAP_SHIFT) {
		if (reg->reg > 0x87fff)
			return -EINVAL;
		tw_writel(reg->reg, reg->val);
	} else {
		__u64 indir_addr = reg->reg - INDIR_SPACE_MAP_SHIFT;

		if (indir_addr > 0xefe)
			return -EINVAL;
		tw_indir_writeb(reg->reg, reg->val);
	}
	return 0;
}