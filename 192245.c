static inline void __skb_frag_ref(skb_frag_t *frag)
{
	get_page(skb_frag_page(frag));
}