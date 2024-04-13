static void skb_release_all(struct sk_buff *skb)
{
	skb_release_head_state(skb);
	if (likely(skb->head))
		skb_release_data(skb);
}