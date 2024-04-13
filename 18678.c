static struct xfrm_policy *xfrm_policy_lookup_bytype(struct net *net, u8 type,
						     const struct flowi *fl,
						     u16 family, u8 dir,
						     u32 if_id)
{
	struct xfrm_pol_inexact_candidates cand;
	const xfrm_address_t *daddr, *saddr;
	struct xfrm_pol_inexact_bin *bin;
	struct xfrm_policy *pol, *ret;
	struct hlist_head *chain;
	unsigned int sequence;
	int err;

	daddr = xfrm_flowi_daddr(fl, family);
	saddr = xfrm_flowi_saddr(fl, family);
	if (unlikely(!daddr || !saddr))
		return NULL;

	rcu_read_lock();
 retry:
	do {
		sequence = read_seqcount_begin(&net->xfrm.xfrm_policy_hash_generation);
		chain = policy_hash_direct(net, daddr, saddr, family, dir);
	} while (read_seqcount_retry(&net->xfrm.xfrm_policy_hash_generation, sequence));

	ret = NULL;
	hlist_for_each_entry_rcu(pol, chain, bydst) {
		err = xfrm_policy_match(pol, fl, type, family, dir, if_id);
		if (err) {
			if (err == -ESRCH)
				continue;
			else {
				ret = ERR_PTR(err);
				goto fail;
			}
		} else {
			ret = pol;
			break;
		}
	}
	bin = xfrm_policy_inexact_lookup_rcu(net, type, family, dir, if_id);
	if (!bin || !xfrm_policy_find_inexact_candidates(&cand, bin, saddr,
							 daddr))
		goto skip_inexact;

	pol = xfrm_policy_eval_candidates(&cand, ret, fl, type,
					  family, dir, if_id);
	if (pol) {
		ret = pol;
		if (IS_ERR(pol))
			goto fail;
	}

skip_inexact:
	if (read_seqcount_retry(&net->xfrm.xfrm_policy_hash_generation, sequence))
		goto retry;

	if (ret && !xfrm_pol_hold_rcu(ret))
		goto retry;
fail:
	rcu_read_unlock();

	return ret;
}