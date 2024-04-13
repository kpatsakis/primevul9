isdn_net_rm(char *name)
{
	u_long flags;
	isdn_net_dev *p;
	isdn_net_dev *q;

	/* Search name in netdev-chain */
	spin_lock_irqsave(&dev->lock, flags);
	p = dev->netdev;
	q = NULL;
	while (p) {
		if (!strcmp(p->dev->name, name)) {
			spin_unlock_irqrestore(&dev->lock, flags);
			return (isdn_net_realrm(p, q));
		}
		q = p;
		p = (isdn_net_dev *) p->next;
	}
	spin_unlock_irqrestore(&dev->lock, flags);
	/* If no more net-devices remain, disable auto-hangup timer */
	if (dev->netdev == NULL)
		isdn_timer_ctrl(ISDN_TIMER_NETHANGUP, 0);
	return -ENODEV;
}