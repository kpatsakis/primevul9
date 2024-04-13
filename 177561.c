isdn_net_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	isdn_net_local *lp = netdev_priv(ndev);
#ifdef CONFIG_ISDN_X25
	struct concap_proto *cprot = lp->netdev->cprot;
/* At this point hard_start_xmit() passes control to the encapsulation
   protocol (if present).
   For X.25 auto-dialing is completly bypassed because:
   - It does not conform with the semantics of a reliable datalink
   service as needed by X.25 PLP.
   - I don't want that the interface starts dialing when the network layer
   sends a message which requests to disconnect the lapb link (or if it
   sends any other message not resulting in data transmission).
   Instead, dialing will be initiated by the encapsulation protocol entity
   when a dl_establish request is received from the upper layer.
*/
	if (cprot && cprot->pops) {
		int ret = cprot->pops->encap_and_xmit(cprot, skb);

		if (ret)
			netif_stop_queue(ndev);
		return ret;
	} else
#endif
		/* auto-dialing xmit function */
	{
#ifdef ISDN_DEBUG_NET_DUMP
		u_char *buf;
#endif
		isdn_net_adjust_hdr(skb, ndev);
#ifdef ISDN_DEBUG_NET_DUMP
		buf = skb->data;
		isdn_dumppkt("S:", buf, skb->len, 40);
#endif

		if (!(lp->flags & ISDN_NET_CONNECTED)) {
			int chi;
			/* only do autodial if allowed by config */
			if (!(ISDN_NET_DIALMODE(*lp) == ISDN_NET_DM_AUTO)) {
				isdn_net_unreachable(ndev, skb, "dial rejected: interface not in dialmode `auto'");
				dev_kfree_skb(skb);
				return NETDEV_TX_OK;
			}
			if (lp->phone[1]) {
				ulong flags;

				if (lp->dialwait_timer <= 0)
					if (lp->dialstarted > 0 && lp->dialtimeout > 0 && time_before(jiffies, lp->dialstarted + lp->dialtimeout + lp->dialwait))
						lp->dialwait_timer = lp->dialstarted + lp->dialtimeout + lp->dialwait;

				if (lp->dialwait_timer > 0) {
					if (time_before(jiffies, lp->dialwait_timer)) {
						isdn_net_unreachable(ndev, skb, "dial rejected: retry-time not reached");
						dev_kfree_skb(skb);
						return NETDEV_TX_OK;
					} else
						lp->dialwait_timer = 0;
				}
				/* Grab a free ISDN-Channel */
				spin_lock_irqsave(&dev->lock, flags);
				if (((chi =
				      isdn_get_free_channel(
					      ISDN_USAGE_NET,
					      lp->l2_proto,
					      lp->l3_proto,
					      lp->pre_device,
					      lp->pre_channel,
					      lp->msn)
					     ) < 0) &&
				    ((chi =
				      isdn_get_free_channel(
					      ISDN_USAGE_NET,
					      lp->l2_proto,
					      lp->l3_proto,
					      lp->pre_device,
					      lp->pre_channel^1,
					      lp->msn)
					    ) < 0)) {
					spin_unlock_irqrestore(&dev->lock, flags);
					isdn_net_unreachable(ndev, skb,
							     "No channel");
					dev_kfree_skb(skb);
					return NETDEV_TX_OK;
				}
				/* Log packet, which triggered dialing */
				if (dev->net_verbose)
					isdn_net_log_skb(skb, lp);
				lp->dialstate = 1;
				/* Connect interface with channel */
				isdn_net_bind_channel(lp, chi);
#ifdef CONFIG_ISDN_PPP
				if (lp->p_encap == ISDN_NET_ENCAP_SYNCPPP) {
					/* no 'first_skb' handling for syncPPP */
					if (isdn_ppp_bind(lp) < 0) {
						dev_kfree_skb(skb);
						isdn_net_unbind_channel(lp);
						spin_unlock_irqrestore(&dev->lock, flags);
						return NETDEV_TX_OK;	/* STN (skb to nirvana) ;) */
					}
#ifdef CONFIG_IPPP_FILTER
					if (isdn_ppp_autodial_filter(skb, lp)) {
						isdn_ppp_free(lp);
						isdn_net_unbind_channel(lp);
						spin_unlock_irqrestore(&dev->lock, flags);
						isdn_net_unreachable(ndev, skb, "dial rejected: packet filtered");
						dev_kfree_skb(skb);
						return NETDEV_TX_OK;
					}
#endif
					spin_unlock_irqrestore(&dev->lock, flags);
					isdn_net_dial();	/* Initiate dialing */
					netif_stop_queue(ndev);
					return NETDEV_TX_BUSY;	/* let upper layer requeue skb packet */
				}
#endif
				/* Initiate dialing */
				spin_unlock_irqrestore(&dev->lock, flags);
				isdn_net_dial();
				isdn_net_device_stop_queue(lp);
				return NETDEV_TX_BUSY;
			} else {
				isdn_net_unreachable(ndev, skb,
						     "No phone number");
				dev_kfree_skb(skb);
				return NETDEV_TX_OK;
			}
		} else {
			/* Device is connected to an ISDN channel */
			netif_trans_update(ndev);
			if (!lp->dialstate) {
				/* ISDN connection is established, try sending */
				int ret;
				ret = (isdn_net_xmit(ndev, skb));
				if (ret) netif_stop_queue(ndev);
				return ret;
			} else
				netif_stop_queue(ndev);
		}
	}
	return NETDEV_TX_BUSY;
}