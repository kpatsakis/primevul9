static int fsl_hv_close(struct inode *inode, struct file *filp)
{
	struct doorbell_queue *dbq = filp->private_data;
	unsigned long flags;

	int ret = 0;

	spin_lock_irqsave(&db_list_lock, flags);
	list_del(&dbq->list);
	spin_unlock_irqrestore(&db_list_lock, flags);

	kfree(dbq);

	return ret;
}