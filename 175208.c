void skb_scrub_packet(struct sk_buff *skb, bool xnet)
{
	if (xnet)
		skb_orphan(skb);
	skb->tstamp.tv64 = 0;
	skb->pkt_type = PACKET_HOST;
	skb->skb_iif = 0;
	skb->local_df = 0;
	skb_dst_drop(skb);
	skb->mark = 0;
	secpath_reset(skb);
	nf_reset(skb);
	nf_reset_trace(skb);
}