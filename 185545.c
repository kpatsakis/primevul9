static int futex_unlock_pi(u32 __user *uaddr, unsigned int flags)
{
	u32 uninitialized_var(curval), uval, vpid = task_pid_vnr(current);
	union futex_key key = FUTEX_KEY_INIT;
	struct futex_hash_bucket *hb;
	struct futex_q *match;
	int ret;

retry:
	if (get_user(uval, uaddr))
		return -EFAULT;
	/*
	 * We release only a lock we actually own:
	 */
	if ((uval & FUTEX_TID_MASK) != vpid)
		return -EPERM;

	ret = get_futex_key(uaddr, flags & FLAGS_SHARED, &key, VERIFY_WRITE);
	if (ret)
		return ret;

	hb = hash_futex(&key);
	spin_lock(&hb->lock);

	/*
	 * Check waiters first. We do not trust user space values at
	 * all and we at least want to know if user space fiddled
	 * with the futex value instead of blindly unlocking.
	 */
	match = futex_top_waiter(hb, &key);
	if (match) {
		ret = wake_futex_pi(uaddr, uval, match, hb);
		/*
		 * In case of success wake_futex_pi dropped the hash
		 * bucket lock.
		 */
		if (!ret)
			goto out_putkey;
		/*
		 * The atomic access to the futex value generated a
		 * pagefault, so retry the user-access and the wakeup:
		 */
		if (ret == -EFAULT)
			goto pi_faulted;
		/*
		 * wake_futex_pi has detected invalid state. Tell user
		 * space.
		 */
		goto out_unlock;
	}

	/*
	 * We have no kernel internal state, i.e. no waiters in the
	 * kernel. Waiters which are about to queue themselves are stuck
	 * on hb->lock. So we can safely ignore them. We do neither
	 * preserve the WAITERS bit not the OWNER_DIED one. We are the
	 * owner.
	 */
	if (cmpxchg_futex_value_locked(&curval, uaddr, uval, 0))
		goto pi_faulted;

	/*
	 * If uval has changed, let user space handle it.
	 */
	ret = (curval == uval) ? 0 : -EAGAIN;

out_unlock:
	spin_unlock(&hb->lock);
out_putkey:
	put_futex_key(&key);
	return ret;

pi_faulted:
	spin_unlock(&hb->lock);
	put_futex_key(&key);

	ret = fault_in_user_writeable(uaddr);
	if (!ret)
		goto retry;

	return ret;
}