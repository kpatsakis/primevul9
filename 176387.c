static int vidioc_s_tuner(struct file *file, void *priv,
					const struct v4l2_tuner *v)
{
	return v->index ? -EINVAL : 0;
}