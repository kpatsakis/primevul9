static __inline__ int isdn_net_device_busy(isdn_net_local *lp)
{
	isdn_net_local *nlp;
	isdn_net_dev *nd;
	unsigned long flags;

	if (!isdn_net_lp_busy(lp))
		return 0;

	if (lp->master)
		nd = ISDN_MASTER_PRIV(lp)->netdev;
	else
		nd = lp->netdev;

	spin_lock_irqsave(&nd->queue_lock, flags);
	nlp = lp->next;
	while (nlp != lp) {
		if (!isdn_net_lp_busy(nlp)) {
			spin_unlock_irqrestore(&nd->queue_lock, flags);
			return 0;
		}
		nlp = nlp->next;
	}
	spin_unlock_irqrestore(&nd->queue_lock, flags);
	return 1;
}