static __sum16 ip_mc_validate_checksum(struct sk_buff *skb)
{
	return skb_checksum_simple_validate(skb);
}