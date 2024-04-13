static int attach_to_pi_state(u32 uval, struct futex_pi_state *pi_state,
			      struct futex_pi_state **ps)
{
	pid_t pid = uval & FUTEX_TID_MASK;

	/*
	 * Userspace might have messed up non-PI and PI futexes [3]
	 */
	if (unlikely(!pi_state))
		return -EINVAL;

	WARN_ON(!atomic_read(&pi_state->refcount));

	/*
	 * Handle the owner died case:
	 */
	if (uval & FUTEX_OWNER_DIED) {
		/*
		 * exit_pi_state_list sets owner to NULL and wakes the
		 * topmost waiter. The task which acquires the
		 * pi_state->rt_mutex will fixup owner.
		 */
		if (!pi_state->owner) {
			/*
			 * No pi state owner, but the user space TID
			 * is not 0. Inconsistent state. [5]
			 */
			if (pid)
				return -EINVAL;
			/*
			 * Take a ref on the state and return success. [4]
			 */
			goto out_state;
		}

		/*
		 * If TID is 0, then either the dying owner has not
		 * yet executed exit_pi_state_list() or some waiter
		 * acquired the rtmutex in the pi state, but did not
		 * yet fixup the TID in user space.
		 *
		 * Take a ref on the state and return success. [6]
		 */
		if (!pid)
			goto out_state;
	} else {
		/*
		 * If the owner died bit is not set, then the pi_state
		 * must have an owner. [7]
		 */
		if (!pi_state->owner)
			return -EINVAL;
	}

	/*
	 * Bail out if user space manipulated the futex value. If pi
	 * state exists then the owner TID must be the same as the
	 * user space TID. [9/10]
	 */
	if (pid != task_pid_vnr(pi_state->owner))
		return -EINVAL;
out_state:
	atomic_inc(&pi_state->refcount);
	*ps = pi_state;
	return 0;
}