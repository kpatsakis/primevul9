int get_compat_sigevent(struct sigevent *event,
		const struct compat_sigevent __user *u_event)
{
	memset(event, 0, sizeof(*event));
	return (!access_ok(u_event, sizeof(*u_event)) ||
		__get_user(event->sigev_value.sival_int,
			&u_event->sigev_value.sival_int) ||
		__get_user(event->sigev_signo, &u_event->sigev_signo) ||
		__get_user(event->sigev_notify, &u_event->sigev_notify) ||
		__get_user(event->sigev_notify_thread_id,
			&u_event->sigev_notify_thread_id))
		? -EFAULT : 0;
}