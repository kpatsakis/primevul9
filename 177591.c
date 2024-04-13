isdn_net_unreachable(struct net_device *dev, struct sk_buff *skb, char *reason)
{
	if (skb) {

		u_short proto = ntohs(skb->protocol);

		printk(KERN_DEBUG "isdn_net: %s: %s, signalling dst_link_failure %s\n",
		       dev->name,
		       (reason != NULL) ? reason : "unknown",
		       (proto != ETH_P_IP) ? "Protocol != ETH_P_IP" : "");

		dst_link_failure(skb);
	}
	else {  /* dial not triggered by rawIP packet */
		printk(KERN_DEBUG "isdn_net: %s: %s\n",
		       dev->name,
		       (reason != NULL) ? reason : "reason unknown");
	}
}