static void mark_wake_futex(struct wake_q_head *wake_q, struct futex_q *q)
{
	struct task_struct *p = q->task;

	if (WARN(q->pi_state || q->rt_waiter, "refusing to wake PI futex\n"))
		return;

	/*
	 * Queue the task for later wakeup for after we've released
	 * the hb->lock. wake_q_add() grabs reference to p.
	 */
	wake_q_add(wake_q, p);
	__unqueue_futex(q);
	/*
	 * The waiting task can free the futex_q as soon as
	 * q->lock_ptr = NULL is written, without taking any locks. A
	 * memory barrier is required here to prevent the following
	 * store to lock_ptr from getting ahead of the plist_del.
	 */
	smp_wmb();
	q->lock_ptr = NULL;
}