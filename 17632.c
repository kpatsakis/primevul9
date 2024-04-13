static int snd_ctl_fasync(int fd, struct file * file, int on)
{
	struct snd_ctl_file *ctl;

	ctl = file->private_data;
	return snd_fasync_helper(fd, file, on, &ctl->fasync);
}