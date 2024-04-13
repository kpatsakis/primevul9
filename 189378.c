static int snd_timer_user_open(struct inode *inode, struct file *file)
{
	struct snd_timer_user *tu;
	int err;

	err = stream_open(inode, file);
	if (err < 0)
		return err;

	tu = kzalloc(sizeof(*tu), GFP_KERNEL);
	if (tu == NULL)
		return -ENOMEM;
	spin_lock_init(&tu->qlock);
	init_waitqueue_head(&tu->qchange_sleep);
	mutex_init(&tu->ioctl_lock);
	tu->ticks = 1;
	if (realloc_user_queue(tu, 128) < 0) {
		kfree(tu);
		return -ENOMEM;
	}
	file->private_data = tu;
	return 0;
}