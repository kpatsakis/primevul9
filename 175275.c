struct sk_buff *skb_morph(struct sk_buff *dst, struct sk_buff *src)
{
	skb_release_all(dst);
	return __skb_clone(dst, src);
}