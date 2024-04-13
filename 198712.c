static int tw5864_frameinterval_get(struct tw5864_input *input,
				    struct v4l2_fract *frameinterval)
{
	struct tw5864_dev *dev = input->root;

	switch (input->std) {
	case STD_NTSC:
		frameinterval->numerator = 1001;
		frameinterval->denominator = 30000;
		break;
	case STD_PAL:
	case STD_SECAM:
		frameinterval->numerator = 1;
		frameinterval->denominator = 25;
		break;
	default:
		dev_warn(&dev->pci->dev, "tw5864_frameinterval_get requested for unknown std %d\n",
			 input->std);
		return -EINVAL;
	}

	return 0;
}