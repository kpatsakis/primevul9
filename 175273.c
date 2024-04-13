void __skb_warn_lro_forwarding(const struct sk_buff *skb)
{
	net_warn_ratelimited("%s: received packets cannot be forwarded while LRO is enabled\n",
			     skb->dev->name);
}