COMPAT_SYSCALL_DEFINE3(timer_create, clockid_t, which_clock,
		       struct compat_sigevent __user *, timer_event_spec,
		       timer_t __user *, created_timer_id)
{
	struct sigevent __user *event = NULL;

	if (timer_event_spec) {
		struct sigevent kevent;

		event = compat_alloc_user_space(sizeof(*event));
		if (get_compat_sigevent(&kevent, timer_event_spec) ||
		    copy_to_user(event, &kevent, sizeof(*event)))
			return -EFAULT;
	}

	return sys_timer_create(which_clock, event, created_timer_id);
}