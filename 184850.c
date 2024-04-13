unsigned int inet_frag_rbtree_purge(struct rb_root *root)
{
	struct rb_node *p = rb_first(root);
	unsigned int sum = 0;

	while (p) {
		struct sk_buff *skb = rb_entry(p, struct sk_buff, rbnode);

		p = rb_next(p);
		rb_erase(&skb->rbnode, root);
		while (skb) {
			struct sk_buff *next = FRAG_CB(skb)->next_frag;

			sum += skb->truesize;
			kfree_skb(skb);
			skb = next;
		}
	}
	return sum;
}