int snd_timer_continue(struct snd_timer_instance *timeri)
{
	/* timer can continue only after pause */
	if (!(timeri->flags & SNDRV_TIMER_IFLG_PAUSED))
		return -EINVAL;

	if (timeri->flags & SNDRV_TIMER_IFLG_SLAVE)
		return snd_timer_start_slave(timeri, false);
	else
		return snd_timer_start1(timeri, false, 0);
}