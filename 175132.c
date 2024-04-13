static void skb_over_panic(struct sk_buff *skb, unsigned int sz, void *addr)
{
	skb_panic(skb, sz, addr, __func__);
}