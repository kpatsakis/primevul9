__xfrm_policy_eval_candidates(struct hlist_head *chain,
			      struct xfrm_policy *prefer,
			      const struct flowi *fl,
			      u8 type, u16 family, int dir, u32 if_id)
{
	u32 priority = prefer ? prefer->priority : ~0u;
	struct xfrm_policy *pol;

	if (!chain)
		return NULL;

	hlist_for_each_entry_rcu(pol, chain, bydst) {
		int err;

		if (pol->priority > priority)
			break;

		err = xfrm_policy_match(pol, fl, type, family, dir, if_id);
		if (err) {
			if (err != -ESRCH)
				return ERR_PTR(err);

			continue;
		}

		if (prefer) {
			/* matches.  Is it older than *prefer? */
			if (pol->priority == priority &&
			    prefer->pos < pol->pos)
				return prefer;
		}

		return pol;
	}

	return NULL;
}