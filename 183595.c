static inline struct ip_sf_list *igmp_mcf_get_first(struct seq_file *seq)
{
	struct net *net = seq_file_net(seq);
	struct ip_sf_list *psf = NULL;
	struct ip_mc_list *im = NULL;
	struct igmp_mcf_iter_state *state = igmp_mcf_seq_private(seq);

	state->idev = NULL;
	state->im = NULL;
	for_each_netdev_rcu(net, state->dev) {
		struct in_device *idev;
		idev = __in_dev_get_rcu(state->dev);
		if (unlikely(!idev))
			continue;
		im = rcu_dereference(idev->mc_list);
		if (likely(im)) {
			spin_lock_bh(&im->lock);
			psf = im->sources;
			if (likely(psf)) {
				state->im = im;
				state->idev = idev;
				break;
			}
			spin_unlock_bh(&im->lock);
		}
	}
	return psf;
}