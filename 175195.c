static void skb_under_panic(struct sk_buff *skb, unsigned int sz, void *addr)
{
	skb_panic(skb, sz, addr, __func__);
}