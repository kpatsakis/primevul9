static inline void skb_frag_list_init(struct sk_buff *skb)
{
	skb_shinfo(skb)->frag_list = NULL;
}