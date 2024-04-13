static inline void skb_frag_unref(struct sk_buff *skb, int f)
{
	__skb_frag_unref(&skb_shinfo(skb)->frags[f]);
}