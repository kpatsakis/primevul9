static int snd_timer_user_continue(struct file *file)
{
	int err;
	struct snd_timer_user *tu;

	tu = file->private_data;
	if (!tu->timeri)
		return -EBADFD;
	/* start timer instead of continue if it's not used before */
	if (!(tu->timeri->flags & SNDRV_TIMER_IFLG_PAUSED))
		return snd_timer_user_start(file);
	tu->timeri->lost = 0;
	err = snd_timer_continue(tu->timeri);
	if (err < 0)
		return err;
	return 0;
}