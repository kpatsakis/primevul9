static int userfaultfd_wake_function(wait_queue_entry_t *wq, unsigned mode,
				     int wake_flags, void *key)
{
	struct userfaultfd_wake_range *range = key;
	int ret;
	struct userfaultfd_wait_queue *uwq;
	unsigned long start, len;

	uwq = container_of(wq, struct userfaultfd_wait_queue, wq);
	ret = 0;
	/* len == 0 means wake all */
	start = range->start;
	len = range->len;
	if (len && (start > uwq->msg.arg.pagefault.address ||
		    start + len <= uwq->msg.arg.pagefault.address))
		goto out;
	WRITE_ONCE(uwq->waken, true);
	/*
	 * The Program-Order guarantees provided by the scheduler
	 * ensure uwq->waken is visible before the task is woken.
	 */
	ret = wake_up_state(wq->private, mode);
	if (ret) {
		/*
		 * Wake only once, autoremove behavior.
		 *
		 * After the effect of list_del_init is visible to the other
		 * CPUs, the waitqueue may disappear from under us, see the
		 * !list_empty_careful() in handle_userfault().
		 *
		 * try_to_wake_up() has an implicit smp_mb(), and the
		 * wq->private is read before calling the extern function
		 * "wake_up_state" (which in turns calls try_to_wake_up).
		 */
		list_del_init(&wq->entry);
	}
out:
	return ret;
}