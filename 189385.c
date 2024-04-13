static int snd_timer_user_start(struct file *file)
{
	int err;
	struct snd_timer_user *tu;

	tu = file->private_data;
	if (!tu->timeri)
		return -EBADFD;
	snd_timer_stop(tu->timeri);
	tu->timeri->lost = 0;
	tu->last_resolution = 0;
	err = snd_timer_start(tu->timeri, tu->ticks);
	if (err < 0)
		return err;
	return 0;
}