xfrm_policy_find_inexact_candidates(struct xfrm_pol_inexact_candidates *cand,
				    struct xfrm_pol_inexact_bin *b,
				    const xfrm_address_t *saddr,
				    const xfrm_address_t *daddr)
{
	struct xfrm_pol_inexact_node *n;
	u16 family;

	if (!b)
		return false;

	family = b->k.family;
	memset(cand, 0, sizeof(*cand));
	cand->res[XFRM_POL_CAND_ANY] = &b->hhead;

	n = xfrm_policy_lookup_inexact_addr(&b->root_d, &b->count, daddr,
					    family);
	if (n) {
		cand->res[XFRM_POL_CAND_DADDR] = &n->hhead;
		n = xfrm_policy_lookup_inexact_addr(&n->root, &b->count, saddr,
						    family);
		if (n)
			cand->res[XFRM_POL_CAND_BOTH] = &n->hhead;
	}

	n = xfrm_policy_lookup_inexact_addr(&b->root_s, &b->count, saddr,
					    family);
	if (n)
		cand->res[XFRM_POL_CAND_SADDR] = &n->hhead;

	return true;
}