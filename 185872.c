static void rt6_probe(struct rt6_info *rt)
{
	struct neighbour *neigh;
	/*
	 * Okay, this does not seem to be appropriate
	 * for now, however, we need to check if it
	 * is really so; aka Router Reachability Probing.
	 *
	 * Router Reachability Probe MUST be rate-limited
	 * to no more than one per minute.
	 */
	if (!rt || !(rt->rt6i_flags & RTF_GATEWAY))
		return;
	rcu_read_lock_bh();
	neigh = __ipv6_neigh_lookup_noref(rt->dst.dev, &rt->rt6i_gateway);
	if (neigh) {
		write_lock(&neigh->lock);
		if (neigh->nud_state & NUD_VALID)
			goto out;
	}

	if (!neigh ||
	    time_after(jiffies, neigh->updated + rt->rt6i_idev->cnf.rtr_probe_interval)) {
		struct __rt6_probe_work *work;

		work = kmalloc(sizeof(*work), GFP_ATOMIC);

		if (neigh && work)
			__neigh_set_probe_once(neigh);

		if (neigh)
			write_unlock(&neigh->lock);

		if (work) {
			INIT_WORK(&work->work, rt6_probe_deferred);
			work->target = rt->rt6i_gateway;
			dev_hold(rt->dst.dev);
			work->dev = rt->dst.dev;
			schedule_work(&work->work);
		}
	} else {
out:
		write_unlock(&neigh->lock);
	}
	rcu_read_unlock_bh();
}