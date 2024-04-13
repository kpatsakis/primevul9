static int snd_timer_user_pause(struct file *file)
{
	int err;
	struct snd_timer_user *tu;

	tu = file->private_data;
	if (!tu->timeri)
		return -EBADFD;
	err = snd_timer_pause(tu->timeri);
	if (err < 0)
		return err;
	return 0;
}