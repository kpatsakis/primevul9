static void copy_skb_header(struct sk_buff *new, const struct sk_buff *old)
{
	__copy_skb_header(new, old);

	skb_shinfo(new)->gso_size = skb_shinfo(old)->gso_size;
	skb_shinfo(new)->gso_segs = skb_shinfo(old)->gso_segs;
	skb_shinfo(new)->gso_type = skb_shinfo(old)->gso_type;
}