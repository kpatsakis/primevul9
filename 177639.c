isdn_net_rcv_skb(int idx, struct sk_buff *skb)
{
	isdn_net_dev *p = dev->rx_netdev[idx];

	if (p) {
		isdn_net_local *lp = p->local;
		if ((lp->flags & ISDN_NET_CONNECTED) &&
		    (!lp->dialstate)) {
			isdn_net_receive(p->dev, skb);
			return 1;
		}
	}
	return 0;
}