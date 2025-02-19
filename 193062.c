SYSCALL_DEFINE3(timer_create, const clockid_t, which_clock,
		struct sigevent __user *, timer_event_spec,
		timer_t __user *, created_timer_id)
{
	const struct k_clock *kc = clockid_to_kclock(which_clock);
	struct k_itimer *new_timer;
	int error, new_timer_id;
	sigevent_t event;
	int it_id_set = IT_ID_NOT_SET;

	if (!kc)
		return -EINVAL;
	if (!kc->timer_create)
		return -EOPNOTSUPP;

	new_timer = alloc_posix_timer();
	if (unlikely(!new_timer))
		return -EAGAIN;

	spin_lock_init(&new_timer->it_lock);
	new_timer_id = posix_timer_add(new_timer);
	if (new_timer_id < 0) {
		error = new_timer_id;
		goto out;
	}

	it_id_set = IT_ID_SET;
	new_timer->it_id = (timer_t) new_timer_id;
	new_timer->it_clock = which_clock;
	new_timer->kclock = kc;
	new_timer->it_overrun = -1;

	if (timer_event_spec) {
		if (copy_from_user(&event, timer_event_spec, sizeof (event))) {
			error = -EFAULT;
			goto out;
		}
		rcu_read_lock();
		new_timer->it_pid = get_pid(good_sigevent(&event));
		rcu_read_unlock();
		if (!new_timer->it_pid) {
			error = -EINVAL;
			goto out;
		}
	} else {
		memset(&event.sigev_value, 0, sizeof(event.sigev_value));
		event.sigev_notify = SIGEV_SIGNAL;
		event.sigev_signo = SIGALRM;
		event.sigev_value.sival_int = new_timer->it_id;
		new_timer->it_pid = get_pid(task_tgid(current));
	}

	new_timer->it_sigev_notify     = event.sigev_notify;
	new_timer->sigq->info.si_signo = event.sigev_signo;
	new_timer->sigq->info.si_value = event.sigev_value;
	new_timer->sigq->info.si_tid   = new_timer->it_id;
	new_timer->sigq->info.si_code  = SI_TIMER;

	if (copy_to_user(created_timer_id,
			 &new_timer_id, sizeof (new_timer_id))) {
		error = -EFAULT;
		goto out;
	}

	error = kc->timer_create(new_timer);
	if (error)
		goto out;

	spin_lock_irq(&current->sighand->siglock);
	new_timer->it_signal = current->signal;
	list_add(&new_timer->list, &current->signal->posix_timers);
	spin_unlock_irq(&current->sighand->siglock);

	return 0;
	/*
	 * In the case of the timer belonging to another task, after
	 * the task is unlocked, the timer is owned by the other task
	 * and may cease to exist at any time.  Don't use or modify
	 * new_timer after the unlock call.
	 */
out:
	release_posix_timer(new_timer, it_id_set);
	return error;
}