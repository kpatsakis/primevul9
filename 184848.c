int nf_ct_frag6_init(void)
{
	int ret = 0;

	nf_frags.constructor = ip6frag_init;
	nf_frags.destructor = NULL;
	nf_frags.qsize = sizeof(struct frag_queue);
	nf_frags.frag_expire = nf_ct_frag6_expire;
	nf_frags.frags_cache_name = nf_frags_cache_name;
	nf_frags.rhash_params = nfct_rhash_params;
	ret = inet_frags_init(&nf_frags);
	if (ret)
		goto out;
	ret = register_pernet_subsys(&nf_ct_net_ops);
	if (ret)
		inet_frags_fini(&nf_frags);

out:
	return ret;
}