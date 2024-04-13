static int fsl_hv_open(struct inode *inode, struct file *filp)
{
	struct doorbell_queue *dbq;
	unsigned long flags;
	int ret = 0;

	dbq = kzalloc(sizeof(struct doorbell_queue), GFP_KERNEL);
	if (!dbq) {
		pr_err("fsl-hv: out of memory\n");
		return -ENOMEM;
	}

	spin_lock_init(&dbq->lock);
	init_waitqueue_head(&dbq->wait);

	spin_lock_irqsave(&db_list_lock, flags);
	list_add(&dbq->list, &db_list);
	spin_unlock_irqrestore(&db_list_lock, flags);

	filp->private_data = dbq;

	return ret;
}