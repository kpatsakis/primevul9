static void ip_expire(struct timer_list *t)
{
	struct inet_frag_queue *frag = from_timer(frag, t, timer);
	const struct iphdr *iph;
	struct sk_buff *head = NULL;
	struct net *net;
	struct ipq *qp;
	int err;

	qp = container_of(frag, struct ipq, q);
	net = container_of(qp->q.net, struct net, ipv4.frags);

	rcu_read_lock();
	spin_lock(&qp->q.lock);

	if (qp->q.flags & INET_FRAG_COMPLETE)
		goto out;

	ipq_kill(qp);
	__IP_INC_STATS(net, IPSTATS_MIB_REASMFAILS);
	__IP_INC_STATS(net, IPSTATS_MIB_REASMTIMEOUT);

	if (!(qp->q.flags & INET_FRAG_FIRST_IN))
		goto out;

	/* sk_buff::dev and sk_buff::rbnode are unionized. So we
	 * pull the head out of the tree in order to be able to
	 * deal with head->dev.
	 */
	if (qp->q.fragments) {
		head = qp->q.fragments;
		qp->q.fragments = head->next;
	} else {
		head = skb_rb_first(&qp->q.rb_fragments);
		if (!head)
			goto out;
		if (FRAG_CB(head)->next_frag)
			rb_replace_node(&head->rbnode,
					&FRAG_CB(head)->next_frag->rbnode,
					&qp->q.rb_fragments);
		else
			rb_erase(&head->rbnode, &qp->q.rb_fragments);
		memset(&head->rbnode, 0, sizeof(head->rbnode));
		barrier();
	}
	if (head == qp->q.fragments_tail)
		qp->q.fragments_tail = NULL;

	sub_frag_mem_limit(qp->q.net, head->truesize);

	head->dev = dev_get_by_index_rcu(net, qp->iif);
	if (!head->dev)
		goto out;


	/* skb has no dst, perform route lookup again */
	iph = ip_hdr(head);
	err = ip_route_input_noref(head, iph->daddr, iph->saddr,
					   iph->tos, head->dev);
	if (err)
		goto out;

	/* Only an end host needs to send an ICMP
	 * "Fragment Reassembly Timeout" message, per RFC792.
	 */
	if (frag_expire_skip_icmp(qp->q.key.v4.user) &&
	    (skb_rtable(head)->rt_type != RTN_LOCAL))
		goto out;

	spin_unlock(&qp->q.lock);
	icmp_send(head, ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, 0);
	goto out_rcu_unlock;

out:
	spin_unlock(&qp->q.lock);
out_rcu_unlock:
	rcu_read_unlock();
	if (head)
		kfree_skb(head);
	ipq_put(qp);
}