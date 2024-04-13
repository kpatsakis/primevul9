mptctl_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long ret;
	mutex_lock(&mpctl_mutex);
	ret = __mptctl_ioctl(file, cmd, arg);
	mutex_unlock(&mpctl_mutex);
	return ret;
}