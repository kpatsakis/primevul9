static int futex_lock_pi_atomic(u32 __user *uaddr, struct futex_hash_bucket *hb,
				union futex_key *key,
				struct futex_pi_state **ps,
				struct task_struct *task, int set_waiters)
{
	int lock_taken, ret, force_take = 0;
	u32 uval, newval, curval, vpid = task_pid_vnr(task);

retry:
	ret = lock_taken = 0;

	/*
	 * To avoid races, we attempt to take the lock here again
	 * (by doing a 0 -> TID atomic cmpxchg), while holding all
	 * the locks. It will most likely not succeed.
	 */
	newval = vpid;
	if (set_waiters)
		newval |= FUTEX_WAITERS;

	if (unlikely(cmpxchg_futex_value_locked(&curval, uaddr, 0, newval)))
		return -EFAULT;

	/*
	 * Detect deadlocks.
	 */
	if ((unlikely((curval & FUTEX_TID_MASK) == vpid)))
		return -EDEADLK;

	/*
	 * Surprise - we got the lock, but we do not trust user space at all.
	 */
	if (unlikely(!curval)) {
		/*
		 * We verify whether there is kernel state for this
		 * futex. If not, we can safely assume, that the 0 ->
		 * TID transition is correct. If state exists, we do
		 * not bother to fixup the user space state as it was
		 * corrupted already.
		 */
		return futex_top_waiter(hb, key) ? -EINVAL : 1;
	}

	uval = curval;

	/*
	 * Set the FUTEX_WAITERS flag, so the owner will know it has someone
	 * to wake at the next unlock.
	 */
	newval = curval | FUTEX_WAITERS;

	/*
	 * Should we force take the futex? See below.
	 */
	if (unlikely(force_take)) {
		/*
		 * Keep the OWNER_DIED and the WAITERS bit and set the
		 * new TID value.
		 */
		newval = (curval & ~FUTEX_TID_MASK) | vpid;
		force_take = 0;
		lock_taken = 1;
	}

	if (unlikely(cmpxchg_futex_value_locked(&curval, uaddr, uval, newval)))
		return -EFAULT;
	if (unlikely(curval != uval))
		goto retry;

	/*
	 * We took the lock due to forced take over.
	 */
	if (unlikely(lock_taken))
		return 1;

	/*
	 * We dont have the lock. Look up the PI state (or create it if
	 * we are the first waiter):
	 */
	ret = lookup_pi_state(uval, hb, key, ps);

	if (unlikely(ret)) {
		switch (ret) {
		case -ESRCH:
			/*
			 * We failed to find an owner for this
			 * futex. So we have no pi_state to block
			 * on. This can happen in two cases:
			 *
			 * 1) The owner died
			 * 2) A stale FUTEX_WAITERS bit
			 *
			 * Re-read the futex value.
			 */
			if (get_futex_value_locked(&curval, uaddr))
				return -EFAULT;

			/*
			 * If the owner died or we have a stale
			 * WAITERS bit the owner TID in the user space
			 * futex is 0.
			 */
			if (!(curval & FUTEX_TID_MASK)) {
				force_take = 1;
				goto retry;
			}
		default:
			break;
		}
	}

	return ret;
}