static int tw5864_input_std_get(struct tw5864_input *input,
				enum tw5864_vid_std *std)
{
	struct tw5864_dev *dev = input->root;
	u8 std_reg = tw_indir_readb(TW5864_INDIR_VIN_E(input->nr));

	*std = (std_reg & 0x70) >> 4;

	if (std_reg & 0x80) {
		dev_dbg(&dev->pci->dev,
			"Video format detection is in progress, please wait\n");
		return -EAGAIN;
	}

	return 0;
}