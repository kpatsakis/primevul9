static struct ip_sf_list *igmp_mcf_get_next(struct seq_file *seq, struct ip_sf_list *psf)
{
	struct igmp_mcf_iter_state *state = igmp_mcf_seq_private(seq);

	psf = psf->sf_next;
	while (!psf) {
		spin_unlock_bh(&state->im->lock);
		state->im = state->im->next;
		while (!state->im) {
			state->dev = next_net_device_rcu(state->dev);
			if (!state->dev) {
				state->idev = NULL;
				goto out;
			}
			state->idev = __in_dev_get_rcu(state->dev);
			if (!state->idev)
				continue;
			state->im = rcu_dereference(state->idev->mc_list);
		}
		if (!state->im)
			break;
		spin_lock_bh(&state->im->lock);
		psf = state->im->sources;
	}
out:
	return psf;
}