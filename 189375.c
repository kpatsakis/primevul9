static void snd_timer_s_function(struct timer_list *t)
{
	struct snd_timer_system_private *priv = from_timer(priv, t,
								tlist);
	struct snd_timer *timer = priv->snd_timer;
	unsigned long jiff = jiffies;
	if (time_after(jiff, priv->last_expires))
		priv->correction += (long)jiff - (long)priv->last_expires;
	snd_timer_interrupt(timer, (long)jiff - (long)priv->last_jiffies);
}