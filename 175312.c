static int skb_checksum_setup_ipv6(struct sk_buff *skb, bool recalculate)
{
	int err;
	u8 nexthdr;
	unsigned int off;
	unsigned int len;
	bool fragment;
	bool done;

	fragment = false;
	done = false;

	off = sizeof(struct ipv6hdr);

	err = skb_maybe_pull_tail(skb, off, MAX_IPV6_HDR_LEN);
	if (err < 0)
		goto out;

	nexthdr = ipv6_hdr(skb)->nexthdr;

	len = sizeof(struct ipv6hdr) + ntohs(ipv6_hdr(skb)->payload_len);
	while (off <= len && !done) {
		switch (nexthdr) {
		case IPPROTO_DSTOPTS:
		case IPPROTO_HOPOPTS:
		case IPPROTO_ROUTING: {
			struct ipv6_opt_hdr *hp;

			err = skb_maybe_pull_tail(skb,
						  off +
						  sizeof(struct ipv6_opt_hdr),
						  MAX_IPV6_HDR_LEN);
			if (err < 0)
				goto out;

			hp = OPT_HDR(struct ipv6_opt_hdr, skb, off);
			nexthdr = hp->nexthdr;
			off += ipv6_optlen(hp);
			break;
		}
		case IPPROTO_AH: {
			struct ip_auth_hdr *hp;

			err = skb_maybe_pull_tail(skb,
						  off +
						  sizeof(struct ip_auth_hdr),
						  MAX_IPV6_HDR_LEN);
			if (err < 0)
				goto out;

			hp = OPT_HDR(struct ip_auth_hdr, skb, off);
			nexthdr = hp->nexthdr;
			off += ipv6_authlen(hp);
			break;
		}
		case IPPROTO_FRAGMENT: {
			struct frag_hdr *hp;

			err = skb_maybe_pull_tail(skb,
						  off +
						  sizeof(struct frag_hdr),
						  MAX_IPV6_HDR_LEN);
			if (err < 0)
				goto out;

			hp = OPT_HDR(struct frag_hdr, skb, off);

			if (hp->frag_off & htons(IP6_OFFSET | IP6_MF))
				fragment = true;

			nexthdr = hp->nexthdr;
			off += sizeof(struct frag_hdr);
			break;
		}
		default:
			done = true;
			break;
		}
	}

	err = -EPROTO;

	if (!done || fragment)
		goto out;

	switch (nexthdr) {
	case IPPROTO_TCP:
		err = skb_maybe_pull_tail(skb,
					  off + sizeof(struct tcphdr),
					  MAX_IPV6_HDR_LEN);
		if (err < 0)
			goto out;

		if (!skb_partial_csum_set(skb, off,
					  offsetof(struct tcphdr, check))) {
			err = -EPROTO;
			goto out;
		}

		if (recalculate)
			tcp_hdr(skb)->check =
				~csum_ipv6_magic(&ipv6_hdr(skb)->saddr,
						 &ipv6_hdr(skb)->daddr,
						 skb->len - off,
						 IPPROTO_TCP, 0);
		break;
	case IPPROTO_UDP:
		err = skb_maybe_pull_tail(skb,
					  off + sizeof(struct udphdr),
					  MAX_IPV6_HDR_LEN);
		if (err < 0)
			goto out;

		if (!skb_partial_csum_set(skb, off,
					  offsetof(struct udphdr, check))) {
			err = -EPROTO;
			goto out;
		}

		if (recalculate)
			udp_hdr(skb)->check =
				~csum_ipv6_magic(&ipv6_hdr(skb)->saddr,
						 &ipv6_hdr(skb)->daddr,
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