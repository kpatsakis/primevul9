static inline void skb_drop_fraglist(struct sk_buff *skb)
{
	skb_drop_list(&skb_shinfo(skb)->frag_list);
}