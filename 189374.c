static long __snd_timer_user_ioctl(struct file *file, unsigned int cmd,
				 unsigned long arg)
{
	struct snd_timer_user *tu;
	void __user *argp = (void __user *)arg;
	int __user *p = argp;

	tu = file->private_data;
	switch (cmd) {
	case SNDRV_TIMER_IOCTL_PVERSION:
		return put_user(SNDRV_TIMER_VERSION, p) ? -EFAULT : 0;
	case SNDRV_TIMER_IOCTL_NEXT_DEVICE:
		return snd_timer_user_next_device(argp);
	case SNDRV_TIMER_IOCTL_TREAD:
	{
		int xarg, old_tread;

		if (tu->timeri)	/* too late */
			return -EBUSY;
		if (get_user(xarg, p))
			return -EFAULT;
		old_tread = tu->tread;
		tu->tread = xarg ? 1 : 0;
		if (tu->tread != old_tread &&
		    realloc_user_queue(tu, tu->queue_size) < 0) {
			tu->tread = old_tread;
			return -ENOMEM;
		}
		return 0;
	}
	case SNDRV_TIMER_IOCTL_GINFO:
		return snd_timer_user_ginfo(file, argp);
	case SNDRV_TIMER_IOCTL_GPARAMS:
		return snd_timer_user_gparams(file, argp);
	case SNDRV_TIMER_IOCTL_GSTATUS:
		return snd_timer_user_gstatus(file, argp);
	case SNDRV_TIMER_IOCTL_SELECT:
		return snd_timer_user_tselect(file, argp);
	case SNDRV_TIMER_IOCTL_INFO:
		return snd_timer_user_info(file, argp);
	case SNDRV_TIMER_IOCTL_PARAMS:
		return snd_timer_user_params(file, argp);
	case SNDRV_TIMER_IOCTL_STATUS:
		return snd_timer_user_status(file, argp);
	case SNDRV_TIMER_IOCTL_START:
	case SNDRV_TIMER_IOCTL_START_OLD:
		return snd_timer_user_start(file);
	case SNDRV_TIMER_IOCTL_STOP:
	case SNDRV_TIMER_IOCTL_STOP_OLD:
		return snd_timer_user_stop(file);
	case SNDRV_TIMER_IOCTL_CONTINUE:
	case SNDRV_TIMER_IOCTL_CONTINUE_OLD:
		return snd_timer_user_continue(file);
	case SNDRV_TIMER_IOCTL_PAUSE:
	case SNDRV_TIMER_IOCTL_PAUSE_OLD:
		return snd_timer_user_pause(file);
	}
	return -ENOTTY;
}