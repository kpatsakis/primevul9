static int snd_ctl_subscribe_events(struct snd_ctl_file *file, int __user *ptr)
{
	int subscribe;
	if (get_user(subscribe, ptr))
		return -EFAULT;
	if (subscribe < 0) {
		subscribe = file->subscribed;
		if (put_user(subscribe, ptr))
			return -EFAULT;
		return 0;
	}
	if (subscribe) {
		file->subscribed = 1;
		return 0;
	} else if (file->subscribed) {
		snd_ctl_empty_read_queue(file);
		file->subscribed = 0;
	}
	return 0;
}