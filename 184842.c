static void ip4_frag_init_run(struct sk_buff *skb)
{
	BUILD_BUG_ON(sizeof(struct ipfrag_skb_cb) > sizeof(skb->cb));

	FRAG_CB(skb)->next_frag = NULL;
	FRAG_CB(skb)->frag_run_len = skb->len;
}