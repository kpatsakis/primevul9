int __xfrm_policy_check(struct sock *sk, int dir, struct sk_buff *skb,
			unsigned short family)
{
	struct net *net = dev_net(skb->dev);
	struct xfrm_policy *pol;
	struct xfrm_policy *pols[XFRM_POLICY_TYPE_MAX];
	int npols = 0;
	int xfrm_nr;
	int pi;
	int reverse;
	struct flowi fl;
	int xerr_idx = -1;
	const struct xfrm_if_cb *ifcb;
	struct sec_path *sp;
	struct xfrm_if *xi;
	u32 if_id = 0;

	rcu_read_lock();
	ifcb = xfrm_if_get_cb();

	if (ifcb) {
		xi = ifcb->decode_session(skb, family);
		if (xi) {
			if_id = xi->p.if_id;
			net = xi->net;
		}
	}
	rcu_read_unlock();

	reverse = dir & ~XFRM_POLICY_MASK;
	dir &= XFRM_POLICY_MASK;

	if (__xfrm_decode_session(skb, &fl, family, reverse) < 0) {
		XFRM_INC_STATS(net, LINUX_MIB_XFRMINHDRERROR);
		return 0;
	}

	nf_nat_decode_session(skb, &fl, family);

	/* First, check used SA against their selectors. */
	sp = skb_sec_path(skb);
	if (sp) {
		int i;

		for (i = sp->len - 1; i >= 0; i--) {
			struct xfrm_state *x = sp->xvec[i];
			if (!xfrm_selector_match(&x->sel, &fl, family)) {
				XFRM_INC_STATS(net, LINUX_MIB_XFRMINSTATEMISMATCH);
				return 0;
			}
		}
	}

	pol = NULL;
	sk = sk_to_full_sk(sk);
	if (sk && sk->sk_policy[dir]) {
		pol = xfrm_sk_policy_lookup(sk, dir, &fl, family, if_id);
		if (IS_ERR(pol)) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMINPOLERROR);
			return 0;
		}
	}

	if (!pol)
		pol = xfrm_policy_lookup(net, &fl, family, dir, if_id);

	if (IS_ERR(pol)) {
		XFRM_INC_STATS(net, LINUX_MIB_XFRMINPOLERROR);
		return 0;
	}

	if (!pol) {
		if (net->xfrm.policy_default[dir] == XFRM_USERPOLICY_BLOCK) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMINNOPOLS);
			return 0;
		}

		if (sp && secpath_has_nontransport(sp, 0, &xerr_idx)) {
			xfrm_secpath_reject(xerr_idx, skb, &fl);
			XFRM_INC_STATS(net, LINUX_MIB_XFRMINNOPOLS);
			return 0;
		}
		return 1;
	}

	pol->curlft.use_time = ktime_get_real_seconds();

	pols[0] = pol;
	npols++;
#ifdef CONFIG_XFRM_SUB_POLICY
	if (pols[0]->type != XFRM_POLICY_TYPE_MAIN) {
		pols[1] = xfrm_policy_lookup_bytype(net, XFRM_POLICY_TYPE_MAIN,
						    &fl, family,
						    XFRM_POLICY_IN, if_id);
		if (pols[1]) {
			if (IS_ERR(pols[1])) {
				XFRM_INC_STATS(net, LINUX_MIB_XFRMINPOLERROR);
				return 0;
			}
			pols[1]->curlft.use_time = ktime_get_real_seconds();
			npols++;
		}
	}
#endif

	if (pol->action == XFRM_POLICY_ALLOW) {
		static struct sec_path dummy;
		struct xfrm_tmpl *tp[XFRM_MAX_DEPTH];
		struct xfrm_tmpl *stp[XFRM_MAX_DEPTH];
		struct xfrm_tmpl **tpp = tp;
		int ti = 0;
		int i, k;

		sp = skb_sec_path(skb);
		if (!sp)
			sp = &dummy;

		for (pi = 0; pi < npols; pi++) {
			if (pols[pi] != pol &&
			    pols[pi]->action != XFRM_POLICY_ALLOW) {
				XFRM_INC_STATS(net, LINUX_MIB_XFRMINPOLBLOCK);
				goto reject;
			}
			if (ti + pols[pi]->xfrm_nr >= XFRM_MAX_DEPTH) {
				XFRM_INC_STATS(net, LINUX_MIB_XFRMINBUFFERERROR);
				goto reject_error;
			}
			for (i = 0; i < pols[pi]->xfrm_nr; i++)
				tpp[ti++] = &pols[pi]->xfrm_vec[i];
		}
		xfrm_nr = ti;

		if (net->xfrm.policy_default[dir] == XFRM_USERPOLICY_BLOCK &&
		    !xfrm_nr) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMINNOSTATES);
			goto reject;
		}

		if (npols > 1) {
			xfrm_tmpl_sort(stp, tpp, xfrm_nr, family);
			tpp = stp;
		}

		/* For each tunnel xfrm, find the first matching tmpl.
		 * For each tmpl before that, find corresponding xfrm.
		 * Order is _important_. Later we will implement
		 * some barriers, but at the moment barriers
		 * are implied between each two transformations.
		 */
		for (i = xfrm_nr-1, k = 0; i >= 0; i--) {
			k = xfrm_policy_ok(tpp[i], sp, k, family);
			if (k < 0) {
				if (k < -1)
					/* "-2 - errored_index" returned */
					xerr_idx = -(2+k);
				XFRM_INC_STATS(net, LINUX_MIB_XFRMINTMPLMISMATCH);
				goto reject;
			}
		}

		if (secpath_has_nontransport(sp, k, &xerr_idx)) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMINTMPLMISMATCH);
			goto reject;
		}

		xfrm_pols_put(pols, npols);
		return 1;
	}
	XFRM_INC_STATS(net, LINUX_MIB_XFRMINPOLBLOCK);

reject:
	xfrm_secpath_reject(xerr_idx, skb, &fl);
reject_error:
	xfrm_pols_put(pols, npols);
	return 0;
}