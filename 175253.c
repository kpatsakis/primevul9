static int skb_checksum_setup_ip(struct sk_buff *skb, bool recalculate)
{
	unsigned int off;
	bool fragment;
	int err;

	fragment = false;

	err = skb_maybe_pull_tail(skb,
				  sizeof(struct iphdr),
				  MAX_IP_HDR_LEN);
	if (err < 0)
		goto out;

	if (ip_hdr(skb)->frag_off & htons(IP_OFFSET | IP_MF))
		fragment = true;

	off = ip_hdrlen(skb);

	err = -EPROTO;

	if (fragment)
		goto out;

	switch (ip_hdr(skb)->protocol) {
	case IPPROTO_TCP:
		err = skb_maybe_pull_tail(skb,
					  off + sizeof(struct tcphdr),
					  MAX_IP_HDR_LEN);
		if (err < 0)
			goto out;

		if (!skb_partial_csum_set(skb, off,
					  offsetof(struct tcphdr, check))) {
			err = -EPROTO;
			goto out;
		}

		if (recalculate)
			tcp_hdr(skb)->check =
				~csum_tcpudp_magic(ip_hdr(skb)->saddr,
						   ip_hdr(skb)->daddr,
						   skb->len - off,
						   IPPROTO_TCP, 0);
		break;
	case IPPROTO_UDP:
		err = skb_maybe_pull_tail(skb,
					  off + sizeof(struct udphdr),
					  MAX_IP_HDR_LEN);
		if (err < 0)
			goto out;

		if (!skb_partial_csum_set(skb, off,
					  offsetof(struct udphdr, check))) {
			err = -EPROTO;
			goto out;
		}

		if (recalculate)
			udp_hdr(skb)->check =
				~csum_tcpudp_magic(ip_hdr(skb)->saddr,
						   ip_hdr(skb)->daddr,
						   skb->len - off,
						   IPPROTO_UDP, 0);
		break;
	default:
		goto out;
	}

	err = 0;

out:
	return err;
}