static ssize_t fsl_hv_read(struct file *filp, char __user *buf, size_t len,
			   loff_t *off)
{
	struct doorbell_queue *dbq = filp->private_data;
	uint32_t __user *p = (uint32_t __user *) buf; /* for put_user() */
	unsigned long flags;
	ssize_t count = 0;

	/* Make sure we stop when the user buffer is full. */
	while (len >= sizeof(uint32_t)) {
		uint32_t dbell;	/* Local copy of doorbell queue data */

		spin_lock_irqsave(&dbq->lock, flags);

		/*
		 * If the queue is empty, then either we're done or we need
		 * to block.  If the application specified O_NONBLOCK, then
		 * we return the appropriate error code.
		 */
		if (dbq->head == dbq->tail) {
			spin_unlock_irqrestore(&dbq->lock, flags);
			if (count)
				break;
			if (filp->f_flags & O_NONBLOCK)
				return -EAGAIN;
			if (wait_event_interruptible(dbq->wait,
						     dbq->head != dbq->tail))
				return -ERESTARTSYS;
			continue;
		}

		/*
		 * Even though we have an smp_wmb() in the ISR, the core
		 * might speculatively execute the "dbell = ..." below while
		 * it's evaluating the if-statement above.  In that case, the
		 * value put into dbell could be stale if the core accepts the
		 * speculation. To prevent that, we need a read memory barrier
		 * here as well.
		 */
		smp_rmb();

		/* Copy the data to a temporary local buffer, because
		 * we can't call copy_to_user() from inside a spinlock
		 */
		dbell = dbq->q[dbq->head];
		dbq->head = nextp(dbq->head);

		spin_unlock_irqrestore(&dbq->lock, flags);

		if (put_user(dbell, p))
			return -EFAULT;
		p++;
		count += sizeof(uint32_t);
		len -= sizeof(uint32_t);
	}

	return count;
}