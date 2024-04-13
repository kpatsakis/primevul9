static void nf_bridge_adjust_skb_data(struct sk_buff *skb)
{
	if (skb->nf_bridge)
		__skb_push(skb, skb->network_header - skb->mac_header);
}