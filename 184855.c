static int __net_init ipv4_frags_init_net(struct net *net)
{
	int res;

	/* Fragment cache limits.
	 *
	 * The fragment memory accounting code, (tries to) account for
	 * the real memory usage, by measuring both the size of frag
	 * queue struct (inet_frag_queue (ipv4:ipq/ipv6:frag_queue))
	 * and the SKB's truesize.
	 *
	 * A 64K fragment consumes 129736 bytes (44*2944)+200
	 * (1500 truesize == 2944, sizeof(struct ipq) == 200)
	 *
	 * We will commit 4MB at one time. Should we cross that limit
	 * we will prune down to 3MB, making room for approx 8 big 64K
	 * fragments 8x128k.
	 */
	net->ipv4.frags.high_thresh = 4 * 1024 * 1024;
	net->ipv4.frags.low_thresh  = 3 * 1024 * 1024;
	/*
	 * Important NOTE! Fragment queue must be destroyed before MSL expires.
	 * RFC791 is wrong proposing to prolongate timer each fragment arrival
	 * by TTL.
	 */
	net->ipv4.frags.timeout = IP_FRAG_TIME;

	net->ipv4.frags.max_dist = 64;
	net->ipv4.frags.f = &ip4_frags;

	res = inet_frags_init_net(&net->ipv4.frags);
	if (res < 0)
		return res;
	res = ip4_frags_ns_ctl_register(net);
	if (res < 0)
		inet_frags_exit_net(&net->ipv4.frags);
	return res;
}