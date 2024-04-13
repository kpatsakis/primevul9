static void nf_ct_frag6_expire(struct timer_list *t)
{
	struct inet_frag_queue *frag = from_timer(frag, t, timer);
	struct frag_queue *fq;
	struct net *net;

	fq = container_of(frag, struct frag_queue, q);
	net = container_of(fq->q.net, struct net, nf_frag.frags);

	ip6frag_expire_frag_queue(net, fq);
}