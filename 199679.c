input_start_timer(struct input_ctx *ictx)
{
	struct timeval	tv = { .tv_sec = 5, .tv_usec = 0 };

	event_del(&ictx->timer);
	event_add(&ictx->timer, &tv);
}