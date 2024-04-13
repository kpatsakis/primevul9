static void rb_wake_up_waiters(struct irq_work *work)
{
	struct rb_irq_work *rbwork = container_of(work, struct rb_irq_work, work);

	wake_up_all(&rbwork->waiters);
	if (rbwork->wakeup_full) {
		rbwork->wakeup_full = false;
		wake_up_all(&rbwork->full_waiters);
	}
}