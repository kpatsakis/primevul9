isdn_net_adjust_hdr(struct sk_buff *skb, struct net_device *dev)
{
	isdn_net_local *lp = netdev_priv(dev);
	if (!skb)
		return;
	if (lp->p_encap == ISDN_NET_ENCAP_ETHER) {
		const int pullsize = skb_network_offset(skb) - ETH_HLEN;
		if (pullsize > 0) {
			printk(KERN_DEBUG "isdn_net: Pull junk %d\n", pullsize);
			skb_pull(skb, pullsize);
		}
	}
}