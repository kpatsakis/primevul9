static void addrconf_verify(unsigned long foo)
{
	unsigned long now, next, next_sec, next_sched;
	struct inet6_ifaddr *ifp;
	int i;

	rcu_read_lock_bh();
	spin_lock(&addrconf_verify_lock);
	now = jiffies;
	next = round_jiffies_up(now + ADDR_CHECK_FREQUENCY);

	del_timer(&addr_chk_timer);

	for (i = 0; i < IN6_ADDR_HSIZE; i++) {
restart:
		hlist_for_each_entry_rcu_bh(ifp,
					 &inet6_addr_lst[i], addr_lst) {
			unsigned long age;

			if (ifp->flags & IFA_F_PERMANENT)
				continue;

			spin_lock(&ifp->lock);
			/* We try to batch several events at once. */
			age = (now - ifp->tstamp + ADDRCONF_TIMER_FUZZ_MINUS) / HZ;

			if (ifp->valid_lft != INFINITY_LIFE_TIME &&
			    age >= ifp->valid_lft) {
				spin_unlock(&ifp->lock);
				in6_ifa_hold(ifp);
				ipv6_del_addr(ifp);
				goto restart;
			} else if (ifp->prefered_lft == INFINITY_LIFE_TIME) {
				spin_unlock(&ifp->lock);
				continue;
			} else if (age >= ifp->prefered_lft) {
				/* jiffies - ifp->tstamp > age >= ifp->prefered_lft */
				int deprecate = 0;

				if (!(ifp->flags&IFA_F_DEPRECATED)) {
					deprecate = 1;
					ifp->flags |= IFA_F_DEPRECATED;
				}

				if (time_before(ifp->tstamp + ifp->valid_lft * HZ, next))
					next = ifp->tstamp + ifp->valid_lft * HZ;

				spin_unlock(&ifp->lock);

				if (deprecate) {
					in6_ifa_hold(ifp);

					ipv6_ifa_notify(0, ifp);
					in6_ifa_put(ifp);
					goto restart;
				}
#ifdef CONFIG_IPV6_PRIVACY
			} else if ((ifp->flags&IFA_F_TEMPORARY) &&
				   !(ifp->flags&IFA_F_TENTATIVE)) {
				unsigned long regen_advance = ifp->idev->cnf.regen_max_retry *
					ifp->idev->cnf.dad_transmits *
					ifp->idev->nd_parms->retrans_time / HZ;

				if (age >= ifp->prefered_lft - regen_advance) {
					struct inet6_ifaddr *ifpub = ifp->ifpub;
					if (time_before(ifp->tstamp + ifp->prefered_lft * HZ, next))
						next = ifp->tstamp + ifp->prefered_lft * HZ;
					if (!ifp->regen_count && ifpub) {
						ifp->regen_count++;
						in6_ifa_hold(ifp);
						in6_ifa_hold(ifpub);
						spin_unlock(&ifp->lock);

						spin_lock(&ifpub->lock);
						ifpub->regen_count = 0;
						spin_unlock(&ifpub->lock);
						ipv6_create_tempaddr(ifpub, ifp);
						in6_ifa_put(ifpub);
						in6_ifa_put(ifp);
						goto restart;
					}
				} else if (time_before(ifp->tstamp + ifp->prefered_lft * HZ - regen_advance * HZ, next))
					next = ifp->tstamp + ifp->prefered_lft * HZ - regen_advance * HZ;
				spin_unlock(&ifp->lock);
#endif
			} else {
				/* ifp->prefered_lft <= ifp->valid_lft */
				if (time_before(ifp->tstamp + ifp->prefered_lft * HZ, next))
					next = ifp->tstamp + ifp->prefered_lft * HZ;
				spin_unlock(&ifp->lock);
			}
		}
	}

	next_sec = round_jiffies_up(next);
	next_sched = next;

	/* If rounded timeout is accurate enough, accept it. */
	if (time_before(next_sec, next + ADDRCONF_TIMER_FUZZ))
		next_sched = next_sec;

	/* And minimum interval is ADDRCONF_TIMER_FUZZ_MAX. */
	if (time_before(next_sched, jiffies + ADDRCONF_TIMER_FUZZ_MAX))
		next_sched = jiffies + ADDRCONF_TIMER_FUZZ_MAX;

	ADBG((KERN_DEBUG "now = %lu, schedule = %lu, rounded schedule = %lu => %lu\n",
	      now, next, next_sec, next_sched));

	addr_chk_timer.expires = next_sched;
	add_timer(&addr_chk_timer);
	spin_unlock(&addrconf_verify_lock);
	rcu_read_unlock_bh();
}