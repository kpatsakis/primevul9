static int nf_ct_net_init(struct net *net)
{
	int res;

	net->nf_frag.frags.high_thresh = IPV6_FRAG_HIGH_THRESH;
	net->nf_frag.frags.low_thresh = IPV6_FRAG_LOW_THRESH;
	net->nf_frag.frags.timeout = IPV6_FRAG_TIMEOUT;
	net->nf_frag.frags.f = &nf_frags;

	res = inet_frags_init_net(&net->nf_frag.frags);
	if (res < 0)
		return res;
	res = nf_ct_frag6_sysctl_register(net);
	if (res < 0)
		inet_frags_exit_net(&net->nf_frag.frags);
	return res;
}