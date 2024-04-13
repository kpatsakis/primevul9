static void nf_ct_net_exit(struct net *net)
{
	nf_ct_frags6_sysctl_unregister(net);
	inet_frags_exit_net(&net->nf_frag.frags);
}