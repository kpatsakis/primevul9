static int __net_init xfrm_net_init(struct net *net)
{
	int rv;

	/* Initialize the per-net locks here */
	spin_lock_init(&net->xfrm.xfrm_state_lock);
	spin_lock_init(&net->xfrm.xfrm_policy_lock);
	seqcount_spinlock_init(&net->xfrm.xfrm_policy_hash_generation, &net->xfrm.xfrm_policy_lock);
	mutex_init(&net->xfrm.xfrm_cfg_mutex);
	net->xfrm.policy_default[XFRM_POLICY_IN] = XFRM_USERPOLICY_ACCEPT;
	net->xfrm.policy_default[XFRM_POLICY_FWD] = XFRM_USERPOLICY_ACCEPT;
	net->xfrm.policy_default[XFRM_POLICY_OUT] = XFRM_USERPOLICY_ACCEPT;

	rv = xfrm_statistics_init(net);
	if (rv < 0)
		goto out_statistics;
	rv = xfrm_state_init(net);
	if (rv < 0)
		goto out_state;
	rv = xfrm_policy_init(net);
	if (rv < 0)
		goto out_policy;
	rv = xfrm_sysctl_init(net);
	if (rv < 0)
		goto out_sysctl;

	return 0;

out_sysctl:
	xfrm_policy_fini(net);
out_policy:
	xfrm_state_fini(net);
out_state:
	xfrm_statistics_fini(net);
out_statistics:
	return rv;
}