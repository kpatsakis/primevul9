__wsum skb_checksum(const struct sk_buff *skb, int offset,
		    int len, __wsum csum)
{
	const struct skb_checksum_ops ops = {
		.update  = csum_partial_ext,
		.combine = csum_block_add_ext,
	};

	return __skb_checksum(skb, offset, len, csum, &ops);
}