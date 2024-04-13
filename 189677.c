static int ping_v4_push_pending_frames(struct sock *sk, struct pingfakehdr *pfh,
				       struct flowi4 *fl4)
{
	struct sk_buff *skb = skb_peek(&sk->sk_write_queue);

	if (!skb)
		return 0;
	pfh->wcheck = csum_partial((char *)&pfh->icmph,
		sizeof(struct icmphdr), pfh->wcheck);
	pfh->icmph.checksum = csum_fold(pfh->wcheck);
	memcpy(icmp_hdr(skb), &pfh->icmph, sizeof(struct icmphdr));
	skb->ip_summed = CHECKSUM_NONE;
	return ip_push_pending_frames(sk, fl4);
}