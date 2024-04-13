smtp_auth_failure_pause(struct smtp_session *s)
{
	struct timeval	tv;

	tv.tv_sec = 0;
	tv.tv_usec = arc4random_uniform(1000000);
	log_trace(TRACE_SMTP, "smtp: timing-attack protection triggered, "
	    "will defer answer for %lu microseconds", tv.tv_usec);
	evtimer_set(&s->pause, smtp_auth_failure_resume, s);
	evtimer_add(&s->pause, &tv);
}