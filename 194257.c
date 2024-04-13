static int cpia2_enum_input(struct file *file, void *fh, struct v4l2_input *i)
{
	if (i->index)
		return -EINVAL;
	strscpy(i->name, "Camera", sizeof(i->name));
	i->type = V4L2_INPUT_TYPE_CAMERA;
	return 0;
}