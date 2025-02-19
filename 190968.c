lookup_pi_state(u32 uval, struct futex_hash_bucket *hb,
		union futex_key *key, struct futex_pi_state **ps)
{
	struct futex_pi_state *pi_state = NULL;
	struct futex_q *this, *next;
	struct task_struct *p;
	pid_t pid = uval & FUTEX_TID_MASK;

	plist_for_each_entry_safe(this, next, &hb->chain, list) {
		if (match_futex(&this->key, key)) {
			/*
			 * Sanity check the waiter before increasing
			 * the refcount and attaching to it.
			 */
			pi_state = this->pi_state;
			/*
			 * Userspace might have messed up non-PI and
			 * PI futexes [3]
			 */
			if (unlikely(!pi_state))
				return -EINVAL;

			WARN_ON(!atomic_read(&pi_state->refcount));

			/*
			 * Handle the owner died case:
			 */
			if (uval & FUTEX_OWNER_DIED) {
				/*
				 * exit_pi_state_list sets owner to NULL and
				 * wakes the topmost waiter. The task which
				 * acquires the pi_state->rt_mutex will fixup
				 * owner.
				 */
				if (!pi_state->owner) {
					/*
					 * No pi state owner, but the user
					 * space TID is not 0. Inconsistent
					 * state. [5]
					 */
					if (pid)
						return -EINVAL;
					/*
					 * Take a ref on the state and
					 * return. [4]
					 */
					goto out_state;
				}

				/*
				 * If TID is 0, then either the dying owner
				 * has not yet executed exit_pi_state_list()
				 * or some waiter acquired the rtmutex in the
				 * pi state, but did not yet fixup the TID in
				 * user space.
				 *
				 * Take a ref on the state and return. [6]
				 */
				if (!pid)
					goto out_state;
			} else {
				/*
				 * If the owner died bit is not set,
				 * then the pi_state must have an
				 * owner. [7]
				 */
				if (!pi_state->owner)
					return -EINVAL;
			}

			/*
			 * Bail out if user space manipulated the
			 * futex value. If pi state exists then the
			 * owner TID must be the same as the user
			 * space TID. [9/10]
			 */
			if (pid != task_pid_vnr(pi_state->owner))
				return -EINVAL;

		out_state:
			atomic_inc(&pi_state->refcount);
			*ps = pi_state;
			return 0;
		}
	}

	/*
	 * We are the first waiter - try to look up the real owner and attach
	 * the new pi_state to it, but bail out when TID = 0 [1]
	 */
	if (!pid)
		return -ESRCH;
	p = futex_find_get_task(pid);
	if (!p)
		return -ESRCH;

	if (!p->mm) {
		put_task_struct(p);
		return -EPERM;
	}

	/*
	 * We need to look at the task state flags to figure out,
	 * whether the task is exiting. To protect against the do_exit
	 * change of the task flags, we do this protected by
	 * p->pi_lock:
	 */
	raw_spin_lock_irq(&p->pi_lock);
	if (unlikely(p->flags & PF_EXITING)) {
		/*
		 * The task is on the way out. When PF_EXITPIDONE is
		 * set, we know that the task has finished the
		 * cleanup:
		 */
		int ret = (p->flags & PF_EXITPIDONE) ? -ESRCH : -EAGAIN;

		raw_spin_unlock_irq(&p->pi_lock);
		put_task_struct(p);
		return ret;
	}

	/*
	 * No existing pi state. First waiter. [2]
	 */
	pi_state = alloc_pi_state();

	/*
	 * Initialize the pi_mutex in locked state and make 'p'
	 * the owner of it:
	 */
	rt_mutex_init_proxy_locked(&pi_state->pi_mutex, p);

	/* Store the key for possible exit cleanups: */
	pi_state->key = *key;

	WARN_ON(!list_empty(&pi_state->list));
	list_add(&pi_state->list, &p->pi_state_list);
	pi_state->owner = p;
	raw_spin_unlock_irq(&p->pi_lock);

	put_task_struct(p);

	*ps = pi_state;

	return 0;
}