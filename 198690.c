static int tw5864_g_std(struct file *file, void *priv, v4l2_std_id *std)
{
	struct tw5864_input *input = video_drvdata(file);

	*std = input->v4l2_std;
	return 0;
}