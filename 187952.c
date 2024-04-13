static __poll_t fsl_hv_poll(struct file *filp, struct poll_table_struct *p)
{
	struct doorbell_queue *dbq = filp->private_data;
	unsigned long flags;
	__poll_t mask;

	spin_lock_irqsave(&dbq->lock, flags);

	poll_wait(filp, &dbq->wait, p);
	mask = (dbq->head == dbq->tail) ? 0 : (EPOLLIN | EPOLLRDNORM);

	spin_unlock_irqrestore(&dbq->lock, flags);

	return mask;
}