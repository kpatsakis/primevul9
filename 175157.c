int skb_checksum_setup(struct sk_buff *skb, bool recalculate)
{
	int err;

	switch (skb->protocol) {
	case htons(ETH_P_IP):
		err = skb_checksum_setup_ip(skb, recalculate);
		break;

	case htons(ETH_P_IPV6):
		err = skb_checksum_setup_ipv6(skb, recalculate);
		break;

	default:
		err = -EPROTO;
		break;
	}

	return err;
}