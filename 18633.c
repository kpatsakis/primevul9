decode_session4(struct sk_buff *skb, struct flowi *fl, bool reverse)
{
	const struct iphdr *iph = ip_hdr(skb);
	int ihl = iph->ihl;
	u8 *xprth = skb_network_header(skb) + ihl * 4;
	struct flowi4 *fl4 = &fl->u.ip4;
	int oif = 0;

	if (skb_dst(skb) && skb_dst(skb)->dev)
		oif = skb_dst(skb)->dev->ifindex;

	memset(fl4, 0, sizeof(struct flowi4));
	fl4->flowi4_mark = skb->mark;
	fl4->flowi4_oif = reverse ? skb->skb_iif : oif;

	fl4->flowi4_proto = iph->protocol;
	fl4->daddr = reverse ? iph->saddr : iph->daddr;
	fl4->saddr = reverse ? iph->daddr : iph->saddr;
	fl4->flowi4_tos = iph->tos & ~INET_ECN_MASK;

	if (!ip_is_fragment(iph)) {
		switch (iph->protocol) {
		case IPPROTO_UDP:
		case IPPROTO_UDPLITE:
		case IPPROTO_TCP:
		case IPPROTO_SCTP:
		case IPPROTO_DCCP:
			if (xprth + 4 < skb->data ||
			    pskb_may_pull(skb, xprth + 4 - skb->data)) {
				__be16 *ports;

				xprth = skb_network_header(skb) + ihl * 4;
				ports = (__be16 *)xprth;

				fl4->fl4_sport = ports[!!reverse];
				fl4->fl4_dport = ports[!reverse];
			}
			break;
		case IPPROTO_ICMP:
			if (xprth + 2 < skb->data ||
			    pskb_may_pull(skb, xprth + 2 - skb->data)) {
				u8 *icmp;

				xprth = skb_network_header(skb) + ihl * 4;
				icmp = xprth;

				fl4->fl4_icmp_type = icmp[0];
				fl4->fl4_icmp_code = icmp[1];
			}
			break;
		case IPPROTO_GRE:
			if (xprth + 12 < skb->data ||
			    pskb_may_pull(skb, xprth + 12 - skb->data)) {
				__be16 *greflags;
				__be32 *gre_hdr;

				xprth = skb_network_header(skb) + ihl * 4;
				greflags = (__be16 *)xprth;
				gre_hdr = (__be32 *)xprth;

				if (greflags[0] & GRE_KEY) {
					if (greflags[0] & GRE_CSUM)
						gre_hdr++;
					fl4->fl4_gre_key = gre_hdr[1];
				}
			}
			break;
		default:
			break;
		}
	}
}