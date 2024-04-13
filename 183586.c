static inline struct ip_mc_list *igmp_mc_get_first(struct seq_file *seq)
{
	struct net *net = seq_file_net(seq);
	struct ip_mc_list *im = NULL;
	struct igmp_mc_iter_state *state = igmp_mc_seq_private(seq);

	state->in_dev = NULL;
	for_each_netdev_rcu(net, state->dev) {
		struct in_device *in_dev;

		in_dev = __in_dev_get_rcu(state->dev);
		if (!in_dev)
			continue;
		im = rcu_dereference(in_dev->mc_list);
		if (im) {
			state->in_dev = in_dev;
			break;
		}
	}
	return im;
}