static int wake_futex_pi(u32 __user *uaddr, u32 uval, struct futex_q *this,
			 struct futex_hash_bucket *hb)
{
	struct task_struct *new_owner;
	struct futex_pi_state *pi_state = this->pi_state;
	u32 uninitialized_var(curval), newval;
	WAKE_Q(wake_q);
	bool deboost;
	int ret = 0;

	if (!pi_state)
		return -EINVAL;

	/*
	 * If current does not own the pi_state then the futex is
	 * inconsistent and user space fiddled with the futex value.
	 */
	if (pi_state->owner != current)
		return -EINVAL;

	raw_spin_lock_irq(&pi_state->pi_mutex.wait_lock);
	new_owner = rt_mutex_next_owner(&pi_state->pi_mutex);

	/*
	 * It is possible that the next waiter (the one that brought
	 * this owner to the kernel) timed out and is no longer
	 * waiting on the lock.
	 */
	if (!new_owner)
		new_owner = this->task;

	/*
	 * We pass it to the next owner. The WAITERS bit is always
	 * kept enabled while there is PI state around. We cleanup the
	 * owner died bit, because we are the owner.
	 */
	newval = FUTEX_WAITERS | task_pid_vnr(new_owner);

	if (unlikely(should_fail_futex(true)))
		ret = -EFAULT;

	if (cmpxchg_futex_value_locked(&curval, uaddr, uval, newval))
		ret = -EFAULT;
	else if (curval != uval)
		ret = -EINVAL;
	if (ret) {
		raw_spin_unlock_irq(&pi_state->pi_mutex.wait_lock);
		return ret;
	}

	raw_spin_lock(&pi_state->owner->pi_lock);
	WARN_ON(list_empty(&pi_state->list));
	list_del_init(&pi_state->list);
	raw_spin_unlock(&pi_state->owner->pi_lock);

	raw_spin_lock(&new_owner->pi_lock);
	WARN_ON(!list_empty(&pi_state->list));
	list_add(&pi_state->list, &new_owner->pi_state_list);
	pi_state->owner = new_owner;
	raw_spin_unlock(&new_owner->pi_lock);

	raw_spin_unlock_irq(&pi_state->pi_mutex.wait_lock);

	deboost = rt_mutex_futex_unlock(&pi_state->pi_mutex, &wake_q);

	/*
	 * First unlock HB so the waiter does not spin on it once he got woken
	 * up. Second wake up the waiter before the priority is adjusted. If we
	 * deboost first (and lose our higher priority), then the task might get
	 * scheduled away before the wake up can take place.
	 */
	spin_unlock(&hb->lock);
	wake_up_q(&wake_q);
	if (deboost)
		rt_mutex_adjust_prio(current);

	return 0;
}