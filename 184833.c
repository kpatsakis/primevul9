void __init ipfrag_init(void)
{
	ip4_frags.constructor = ip4_frag_init;
	ip4_frags.destructor = ip4_frag_free;
	ip4_frags.qsize = sizeof(struct ipq);
	ip4_frags.frag_expire = ip_expire;
	ip4_frags.frags_cache_name = ip_frag_cache_name;
	ip4_frags.rhash_params = ip4_rhash_params;
	if (inet_frags_init(&ip4_frags))
		panic("IP: failed to allocate ip4_frags cache\n");
	ip4_frags_ctl_register();
	register_pernet_subsys(&ip4_frags_ops);
}