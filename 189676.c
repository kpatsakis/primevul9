int ping_getfrag(void *from, char *to,
		 int offset, int fraglen, int odd, struct sk_buff *skb)
{
	struct pingfakehdr *pfh = (struct pingfakehdr *)from;

	if (offset == 0) {
		fraglen -= sizeof(struct icmphdr);
		if (fraglen < 0)
			BUG();
		if (!csum_and_copy_from_iter_full(to + sizeof(struct icmphdr),
			    fraglen, &pfh->wcheck,
			    &pfh->msg->msg_iter))
			return -EFAULT;
	} else if (offset < sizeof(struct icmphdr)) {
			BUG();
	} else {
		if (!csum_and_copy_from_iter_full(to, fraglen, &pfh->wcheck,
					    &pfh->msg->msg_iter))
			return -EFAULT;
	}

#if IS_ENABLED(CONFIG_IPV6)
	/* For IPv6, checksum each skb as we go along, as expected by
	 * icmpv6_push_pending_frames. For IPv4, accumulate the checksum in
	 * wcheck, it will be finalized in ping_v4_push_pending_frames.
	 */
	if (pfh->family == AF_INET6) {
		skb->csum = pfh->wcheck;
		skb->ip_summed = CHECKSUM_NONE;
		pfh->wcheck = 0;
	}
#endif

	return 0;
}