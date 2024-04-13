isdn_net_force_dial_lp(isdn_net_local *lp)
{
	if ((!(lp->flags & ISDN_NET_CONNECTED)) && !lp->dialstate) {
		int chi;
		if (lp->phone[1]) {
			ulong flags;

			/* Grab a free ISDN-Channel */
			spin_lock_irqsave(&dev->lock, flags);
			if ((chi = isdn_get_free_channel(
				     ISDN_USAGE_NET,
				     lp->l2_proto,
				     lp->l3_proto,
				     lp->pre_device,
				     lp->pre_channel,
				     lp->msn)) < 0) {
				printk(KERN_WARNING "isdn_net_force_dial: No channel for %s\n",
				       lp->netdev->dev->name);
				spin_unlock_irqrestore(&dev->lock, flags);
				return -EAGAIN;
			}
			lp->dialstate = 1;
			/* Connect interface with channel */
			isdn_net_bind_channel(lp, chi);
#ifdef CONFIG_ISDN_PPP
			if (lp->p_encap == ISDN_NET_ENCAP_SYNCPPP)
				if (isdn_ppp_bind(lp) < 0) {
					isdn_net_unbind_channel(lp);
					spin_unlock_irqrestore(&dev->lock, flags);
					return -EAGAIN;
				}
#endif
			/* Initiate dialing */
			spin_unlock_irqrestore(&dev->lock, flags);
			isdn_net_dial();
			return 0;
		} else
			return -EINVAL;
	} else
		return -EBUSY;
}