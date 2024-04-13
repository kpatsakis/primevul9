xfrm_policy_eval_candidates(struct xfrm_pol_inexact_candidates *cand,
			    struct xfrm_policy *prefer,
			    const struct flowi *fl,
			    u8 type, u16 family, int dir, u32 if_id)
{
	struct xfrm_policy *tmp;
	int i;

	for (i = 0; i < ARRAY_SIZE(cand->res); i++) {
		tmp = __xfrm_policy_eval_candidates(cand->res[i],
						    prefer,
						    fl, type, family, dir,
						    if_id);
		if (!tmp)
			continue;

		if (IS_ERR(tmp))
			return tmp;
		prefer = tmp;
	}

	return prefer;
}