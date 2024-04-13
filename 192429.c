static inline u32 ___skb_get_hash(const struct sk_buff *skb,
				  struct flow_keys *keys, u32 keyval)
{
	if (!skb_flow_dissect_flow_keys(skb, keys))
		return 0;

	return __flow_hash_from_keys(keys, keyval);
}