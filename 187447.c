static int mdesc_open(struct inode *inode, struct file *file)
{
	struct mdesc_handle *hp = mdesc_grab();

	if (!hp)
		return -ENODEV;

	file->private_data = hp;

	return 0;
}