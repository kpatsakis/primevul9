static void ip4_frag_append_to_last_run(struct inet_frag_queue *q,
					struct sk_buff *skb)
{
	RB_CLEAR_NODE(&skb->rbnode);
	FRAG_CB(skb)->next_frag = NULL;

	FRAG_CB(q->last_run_head)->frag_run_len += skb->len;
	FRAG_CB(q->fragments_tail)->next_frag = skb;
	q->fragments_tail = skb;
}