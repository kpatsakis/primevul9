static void ip4_frag_init(struct inet_frag_queue *q, const void *a)
{
	struct ipq *qp = container_of(q, struct ipq, q);
	struct netns_ipv4 *ipv4 = container_of(q->net, struct netns_ipv4,
					       frags);
	struct net *net = container_of(ipv4, struct net, ipv4);

	const struct frag_v4_compare_key *key = a;

	q->key.v4 = *key;
	qp->ecn = 0;
	qp->peer = q->net->max_dist ?
		inet_getpeer_v4(net->ipv4.peers, key->saddr, key->vif, 1) :
		NULL;
}