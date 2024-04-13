static int acm_wb_is_avail(struct acm *acm)
{
	int i, n;
	unsigned long flags;

	n = ACM_NW;
	spin_lock_irqsave(&acm->write_lock, flags);
	for (i = 0; i < ACM_NW; i++)
		n -= acm->wb[i].use;
	spin_unlock_irqrestore(&acm->write_lock, flags);
	return n;
}