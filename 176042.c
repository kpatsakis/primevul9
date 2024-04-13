static inline struct userfaultfd_wait_queue *find_userfault_in(
		wait_queue_head_t *wqh)
{
	wait_queue_entry_t *wq;
	struct userfaultfd_wait_queue *uwq;

	VM_BUG_ON(!spin_is_locked(&wqh->lock));

	uwq = NULL;
	if (!waitqueue_active(wqh))
		goto out;
	/* walk in reverse to provide FIFO behavior to read userfaults */
	wq = list_last_entry(&wqh->head, typeof(*wq), entry);
	uwq = container_of(wq, struct userfaultfd_wait_queue, wq);
out:
	return uwq;
}