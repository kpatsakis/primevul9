isdn_net_realrm(isdn_net_dev *p, isdn_net_dev *q)
{
	u_long flags;

	if (isdn_net_device_started(p)) {
		return -EBUSY;
	}
#ifdef CONFIG_ISDN_X25
	if (p->cprot && p->cprot->pops)
		p->cprot->pops->proto_del(p->cprot);
#endif
	/* Free all phone-entries */
	isdn_net_rmallphone(p);
	/* If interface is bound exclusive, free channel-usage */
	if (p->local->exclusive != -1)
		isdn_unexclusive_channel(p->local->pre_device, p->local->pre_channel);
	if (p->local->master) {
		/* It's a slave-device, so update master's slave-pointer if necessary */
		if (((isdn_net_local *) ISDN_MASTER_PRIV(p->local))->slave ==
		    p->dev)
			((isdn_net_local *)ISDN_MASTER_PRIV(p->local))->slave =
				p->local->slave;
	} else {
		/* Unregister only if it's a master-device */
		unregister_netdev(p->dev);
	}
	/* Unlink device from chain */
	spin_lock_irqsave(&dev->lock, flags);
	if (q)
		q->next = p->next;
	else
		dev->netdev = p->next;
	if (p->local->slave) {
		/* If this interface has a slave, remove it also */
		char *slavename = p->local->slave->name;
		isdn_net_dev *n = dev->netdev;
		q = NULL;
		while (n) {
			if (!strcmp(n->dev->name, slavename)) {
				spin_unlock_irqrestore(&dev->lock, flags);
				isdn_net_realrm(n, q);
				spin_lock_irqsave(&dev->lock, flags);
				break;
			}
			q = n;
			n = (isdn_net_dev *)n->next;
		}
	}
	spin_unlock_irqrestore(&dev->lock, flags);
	/* If no more net-devices remain, disable auto-hangup timer */
	if (dev->netdev == NULL)
		isdn_timer_ctrl(ISDN_TIMER_NETHANGUP, 0);
	free_netdev(p->dev);
	kfree(p);

	return 0;
}