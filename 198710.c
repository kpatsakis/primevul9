static int tw5864_querystd(struct file *file, void *priv, v4l2_std_id *std)
{
	struct tw5864_input *input = video_drvdata(file);
	enum tw5864_vid_std tw_std;
	int ret;

	ret = tw5864_input_std_get(input, &tw_std);
	if (ret)
		return ret;
	*std = tw5864_get_v4l2_std(tw_std);

	return 0;
}