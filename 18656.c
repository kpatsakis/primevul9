static struct xfrm_dst *xfrm_bundle_lookup(struct net *net,
					   const struct flowi *fl,
					   u16 family, u8 dir,
					   struct xfrm_flo *xflo, u32 if_id)
{
	struct xfrm_policy *pols[XFRM_POLICY_TYPE_MAX];
	int num_pols = 0, num_xfrms = 0, err;
	struct xfrm_dst *xdst;

	/* Resolve policies to use if we couldn't get them from
	 * previous cache entry */
	num_pols = 1;
	pols[0] = xfrm_policy_lookup(net, fl, family, dir, if_id);
	err = xfrm_expand_policies(fl, family, pols,
					   &num_pols, &num_xfrms);
	if (err < 0)
		goto inc_error;
	if (num_pols == 0)
		return NULL;
	if (num_xfrms <= 0)
		goto make_dummy_bundle;

	xdst = xfrm_resolve_and_create_bundle(pols, num_pols, fl, family,
					      xflo->dst_orig);
	if (IS_ERR(xdst)) {
		err = PTR_ERR(xdst);
		if (err == -EREMOTE) {
			xfrm_pols_put(pols, num_pols);
			return NULL;
		}

		if (err != -EAGAIN)
			goto error;
		goto make_dummy_bundle;
	} else if (xdst == NULL) {
		num_xfrms = 0;
		goto make_dummy_bundle;
	}

	return xdst;

make_dummy_bundle:
	/* We found policies, but there's no bundles to instantiate:
	 * either because the policy blocks, has no transformations or
	 * we could not build template (no xfrm_states).*/
	xdst = xfrm_create_dummy_bundle(net, xflo, fl, num_xfrms, family);
	if (IS_ERR(xdst)) {
		xfrm_pols_put(pols, num_pols);
		return ERR_CAST(xdst);
	}
	xdst->num_pols = num_pols;
	xdst->num_xfrms = num_xfrms;
	memcpy(xdst->pols, pols, sizeof(struct xfrm_policy *) * num_pols);

	return xdst;

inc_error:
	XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTPOLERROR);
error:
	xfrm_pols_put(pols, num_pols);
	return ERR_PTR(err);
}