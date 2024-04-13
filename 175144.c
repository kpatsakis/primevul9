static void nf_bridge_adjust_segmented_data(struct sk_buff *skb)
{
	if (skb->nf_bridge)
		__skb_pull(skb, skb->network_header - skb->mac_header);
}