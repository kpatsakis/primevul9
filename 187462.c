static ssize_t mdesc_read(struct file *file, char __user *buf,
			  size_t len, loff_t *offp)
{
	struct mdesc_handle *hp = file->private_data;
	unsigned char *mdesc;
	int bytes_left, count = len;

	if (*offp >= hp->handle_size)
		return 0;

	bytes_left = hp->handle_size - *offp;
	if (count > bytes_left)
		count = bytes_left;

	mdesc = (unsigned char *)&hp->mdesc;
	mdesc += *offp;
	if (!copy_to_user(buf, mdesc, count)) {
		*offp += count;
		return count;
	} else {
		return -EFAULT;
	}
}