static unsigned long snd_timer_hw_resolution(struct snd_timer *timer)
{
	if (timer->hw.c_resolution)
		return timer->hw.c_resolution(timer);
	else
		return timer->hw.resolution;
}