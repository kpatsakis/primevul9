static int futex_lock_pi_atomic(u32 __user *uaddr, struct futex_hash_bucket *hb,
				union futex_key *key,
				struct futex_pi_state **ps,
				struct task_struct *task, int set_waiters)
{
	u32 uval, newval, vpid = task_pid_vnr(task);
	struct futex_q *match;
	int ret;

	/*
	 * Read the user space value first so we can validate a few
	 * things before proceeding further.
	 */
	if (get_futex_value_locked(&uval, uaddr))
		return -EFAULT;

	if (unlikely(should_fail_futex(true)))
		return -EFAULT;

	/*
	 * Detect deadlocks.
	 */
	if ((unlikely((uval & FUTEX_TID_MASK) == vpid)))
		return -EDEADLK;

	if ((unlikely(should_fail_futex(true))))
		return -EDEADLK;

	/*
	 * Lookup existing state first. If it exists, try to attach to
	 * its pi_state.
	 */
	match = futex_top_waiter(hb, key);
	if (match)
		return attach_to_pi_state(uval, match->pi_state, ps);

	/*
	 * No waiter and user TID is 0. We are here because the
	 * waiters or the owner died bit is set or called from
	 * requeue_cmp_pi or for whatever reason something took the
	 * syscall.
	 */
	if (!(uval & FUTEX_TID_MASK)) {
		/*
		 * We take over the futex. No other waiters and the user space
		 * TID is 0. We preserve the owner died bit.
		 */
		newval = uval & FUTEX_OWNER_DIED;
		newval |= vpid;

		/* The futex requeue_pi code can enforce the waiters bit */
		if (set_waiters)
			newval |= FUTEX_WAITERS;

		ret = lock_pi_update_atomic(uaddr, uval, newval);
		/* If the take over worked, return 1 */
		return ret < 0 ? ret : 1;
	}

	/*
	 * First waiter. Set the waiters bit before attaching ourself to
	 * the owner. If owner tries to unlock, it will be forced into
	 * the kernel and blocked on hb->lock.
	 */
	newval = uval | FUTEX_WAITERS;
	ret = lock_pi_update_atomic(uaddr, uval, newval);
	if (ret)
		return ret;
	/*
	 * If the update of the user space value succeeded, we try to
	 * attach to the owner. If that fails, no harm done, we only
	 * set the FUTEX_WAITERS bit in the user space variable.
	 */
	return attach_to_pi_owner(uval, key, ps);
}