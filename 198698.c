static int tw5864_s_std(struct file *file, void *priv, v4l2_std_id std)
{
	struct tw5864_input *input = video_drvdata(file);
	struct tw5864_dev *dev = input->root;

	input->v4l2_std = std;
	input->std = tw5864_from_v4l2_std(std);
	tw_indir_writeb(TW5864_INDIR_VIN_E(input->nr), input->std);
	return 0;
}