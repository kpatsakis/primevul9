static int tw5864_g_reg(struct file *file, void *fh,
			struct v4l2_dbg_register *reg)
{
	struct tw5864_input *input = video_drvdata(file);
	struct tw5864_dev *dev = input->root;

	if (reg->reg < INDIR_SPACE_MAP_SHIFT) {
		if (reg->reg > 0x87fff)
			return -EINVAL;
		reg->size = 4;
		reg->val = tw_readl(reg->reg);
	} else {
		__u64 indir_addr = reg->reg - INDIR_SPACE_MAP_SHIFT;

		if (indir_addr > 0xefe)
			return -EINVAL;
		reg->size = 1;
		reg->val = tw_indir_readb(reg->reg);
	}
	return 0;
}