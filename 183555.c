static struct ip_mc_list *igmp_mc_get_next(struct seq_file *seq, struct ip_mc_list *im)
{
	struct igmp_mc_iter_state *state = igmp_mc_seq_private(seq);

	im = rcu_dereference(im->next_rcu);
	while (!im) {
		state->dev = next_net_device_rcu(state->dev);
		if (!state->dev) {
			state->in_dev = NULL;
			break;
		}
		state->in_dev = __in_dev_get_rcu(state->dev);
		if (!state->in_dev)
			continue;
		im = rcu_dereference(state->in_dev->mc_list);
	}
	return im;
}