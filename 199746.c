static struct inet6_ifaddr *if6_get_next(struct seq_file *seq,
					 struct inet6_ifaddr *ifa)
{
	struct if6_iter_state *state = seq->private;
	struct net *net = seq_file_net(seq);

	hlist_for_each_entry_continue_rcu_bh(ifa, addr_lst) {
		if (!net_eq(dev_net(ifa->idev->dev), net))
			continue;
		state->offset++;
		return ifa;
	}

	while (++state->bucket < IN6_ADDR_HSIZE) {
		state->offset = 0;
		hlist_for_each_entry_rcu_bh(ifa,
				     &inet6_addr_lst[state->bucket], addr_lst) {
			if (!net_eq(dev_net(ifa->idev->dev), net))
				continue;
			state->offset++;
			return ifa;
		}
	}

	return NULL;
}