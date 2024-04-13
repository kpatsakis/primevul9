void mce_log(struct mce *m)
{
	if (!mce_gen_pool_add(m))
		irq_work_queue(&mce_irq_work);
}