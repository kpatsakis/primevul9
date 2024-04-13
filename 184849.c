static void ip4_frag_create_run(struct inet_frag_queue *q, struct sk_buff *skb)
{
	if (q->last_run_head)
		rb_link_node(&skb->rbnode, &q->last_run_head->rbnode,
			     &q->last_run_head->rbnode.rb_right);
	else
		rb_link_node(&skb->rbnode, NULL, &q->rb_fragments.rb_node);
	rb_insert_color(&skb->rbnode, &q->rb_fragments);

	ip4_frag_init_run(skb);
	q->fragments_tail = skb;
	q->last_run_head = skb;
}