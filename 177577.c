isdn_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret;

	mutex_lock(&isdn_mutex);
	ret = isdn_ioctl(file, cmd, arg);
	mutex_unlock(&isdn_mutex);

	return ret;
}