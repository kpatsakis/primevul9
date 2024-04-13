static inline struct sk_buff *packet_alloc_skb(struct sock *sk, size_t prepad,
					       size_t reserve, size_t len,
					       size_t linear, int noblock,
					       int *err)
{
	struct sk_buff *skb;

	/* Under a page?  Don't bother with paged skb. */
	if (prepad + len < PAGE_SIZE || !linear)
		linear = len;

	skb = sock_alloc_send_pskb(sk, prepad + linear, len - linear, noblock,
				   err);
	if (!skb)
		return NULL;

	skb_reserve(skb, reserve);
	skb_put(skb, linear);
	skb->data_len = len - linear;
	skb->len += len - linear;

	return skb;
}