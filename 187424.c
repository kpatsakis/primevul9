struct mdesc_handle *mdesc_grab(void)
{
	struct mdesc_handle *hp;
	unsigned long flags;

	spin_lock_irqsave(&mdesc_lock, flags);
	hp = cur_mdesc;
	if (hp)
		refcount_inc(&hp->refcnt);
	spin_unlock_irqrestore(&mdesc_lock, flags);

	return hp;
}