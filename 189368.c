static __poll_t snd_timer_user_poll(struct file *file, poll_table * wait)
{
        __poll_t mask;
        struct snd_timer_user *tu;

        tu = file->private_data;

        poll_wait(file, &tu->qchange_sleep, wait);

	mask = 0;
	spin_lock_irq(&tu->qlock);
	if (tu->qused)
		mask |= EPOLLIN | EPOLLRDNORM;
	if (tu->disconnected)
		mask |= EPOLLERR;
	spin_unlock_irq(&tu->qlock);

	return mask;
}