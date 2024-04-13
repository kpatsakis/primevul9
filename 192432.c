void __skb_get_hash(struct sk_buff *skb)
{
	struct flow_keys keys;
	u32 hash;

	__flow_hash_secret_init();

	hash = ___skb_get_hash(skb, &keys, hashrnd);
	if (!hash)
		return;
	if (keys.ports.ports)
		skb->l4_hash = 1;
	skb->sw_hash = 1;
	skb->hash = hash;
}