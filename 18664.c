static void xfrm_hash_rebuild(struct work_struct *work)
{
	struct net *net = container_of(work, struct net,
				       xfrm.policy_hthresh.work);
	unsigned int hmask;
	struct xfrm_policy *pol;
	struct xfrm_policy *policy;
	struct hlist_head *chain;
	struct hlist_head *odst;
	struct hlist_node *newpos;
	int i;
	int dir;
	unsigned seq;
	u8 lbits4, rbits4, lbits6, rbits6;

	mutex_lock(&hash_resize_mutex);

	/* read selector prefixlen thresholds */
	do {
		seq = read_seqbegin(&net->xfrm.policy_hthresh.lock);

		lbits4 = net->xfrm.policy_hthresh.lbits4;
		rbits4 = net->xfrm.policy_hthresh.rbits4;
		lbits6 = net->xfrm.policy_hthresh.lbits6;
		rbits6 = net->xfrm.policy_hthresh.rbits6;
	} while (read_seqretry(&net->xfrm.policy_hthresh.lock, seq));

	spin_lock_bh(&net->xfrm.xfrm_policy_lock);
	write_seqcount_begin(&net->xfrm.xfrm_policy_hash_generation);

	/* make sure that we can insert the indirect policies again before
	 * we start with destructive action.
	 */
	list_for_each_entry(policy, &net->xfrm.policy_all, walk.all) {
		struct xfrm_pol_inexact_bin *bin;
		u8 dbits, sbits;

		dir = xfrm_policy_id2dir(policy->index);
		if (policy->walk.dead || dir >= XFRM_POLICY_MAX)
			continue;

		if ((dir & XFRM_POLICY_MASK) == XFRM_POLICY_OUT) {
			if (policy->family == AF_INET) {
				dbits = rbits4;
				sbits = lbits4;
			} else {
				dbits = rbits6;
				sbits = lbits6;
			}
		} else {
			if (policy->family == AF_INET) {
				dbits = lbits4;
				sbits = rbits4;
			} else {
				dbits = lbits6;
				sbits = rbits6;
			}
		}

		if (policy->selector.prefixlen_d < dbits ||
		    policy->selector.prefixlen_s < sbits)
			continue;

		bin = xfrm_policy_inexact_alloc_bin(policy, dir);
		if (!bin)
			goto out_unlock;

		if (!xfrm_policy_inexact_alloc_chain(bin, policy, dir))
			goto out_unlock;
	}

	/* reset the bydst and inexact table in all directions */
	for (dir = 0; dir < XFRM_POLICY_MAX; dir++) {
		struct hlist_node *n;

		hlist_for_each_entry_safe(policy, n,
					  &net->xfrm.policy_inexact[dir],
					  bydst_inexact_list) {
			hlist_del_rcu(&policy->bydst);
			hlist_del_init(&policy->bydst_inexact_list);
		}

		hmask = net->xfrm.policy_bydst[dir].hmask;
		odst = net->xfrm.policy_bydst[dir].table;
		for (i = hmask; i >= 0; i--) {
			hlist_for_each_entry_safe(policy, n, odst + i, bydst)
				hlist_del_rcu(&policy->bydst);
		}
		if ((dir & XFRM_POLICY_MASK) == XFRM_POLICY_OUT) {
			/* dir out => dst = remote, src = local */
			net->xfrm.policy_bydst[dir].dbits4 = rbits4;
			net->xfrm.policy_bydst[dir].sbits4 = lbits4;
			net->xfrm.policy_bydst[dir].dbits6 = rbits6;
			net->xfrm.policy_bydst[dir].sbits6 = lbits6;
		} else {
			/* dir in/fwd => dst = local, src = remote */
			net->xfrm.policy_bydst[dir].dbits4 = lbits4;
			net->xfrm.policy_bydst[dir].sbits4 = rbits4;
			net->xfrm.policy_bydst[dir].dbits6 = lbits6;
			net->xfrm.policy_bydst[dir].sbits6 = rbits6;
		}
	}

	/* re-insert all policies by order of creation */
	list_for_each_entry_reverse(policy, &net->xfrm.policy_all, walk.all) {
		if (policy->walk.dead)
			continue;
		dir = xfrm_policy_id2dir(policy->index);
		if (dir >= XFRM_POLICY_MAX) {
			/* skip socket policies */
			continue;
		}
		newpos = NULL;
		chain = policy_hash_bysel(net, &policy->selector,
					  policy->family, dir);

		if (!chain) {
			void *p = xfrm_policy_inexact_insert(policy, dir, 0);

			WARN_ONCE(IS_ERR(p), "reinsert: %ld\n", PTR_ERR(p));
			continue;
		}

		hlist_for_each_entry(pol, chain, bydst) {
			if (policy->priority >= pol->priority)
				newpos = &pol->bydst;
			else
				break;
		}
		if (newpos)
			hlist_add_behind_rcu(&policy->bydst, newpos);
		else
			hlist_add_head_rcu(&policy->bydst, chain);
	}

out_unlock:
	__xfrm_policy_inexact_flush(net);
	write_seqcount_end(&net->xfrm.xfrm_policy_hash_generation);
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);

	mutex_unlock(&hash_resize_mutex);
}