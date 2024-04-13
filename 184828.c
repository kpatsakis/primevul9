static int ip_frag_too_far(struct ipq *qp)
{
	struct inet_peer *peer = qp->peer;
	unsigned int max = qp->q.net->max_dist;
	unsigned int start, end;

	int rc;

	if (!peer || !max)
		return 0;

	start = qp->rid;
	end = atomic_inc_return(&peer->rid);
	qp->rid = end;

	rc = qp->q.fragments_tail && (end - start) > max;

	if (rc) {
		struct net *net;

		net = container_of(qp->q.net, struct net, ipv4.frags);
		__IP_INC_STATS(net, IPSTATS_MIB_REASMFAILS);
	}

	return rc;
}