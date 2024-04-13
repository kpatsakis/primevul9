xfrm_resolve_and_create_bundle(struct xfrm_policy **pols, int num_pols,
			       const struct flowi *fl, u16 family,
			       struct dst_entry *dst_orig)
{
	struct net *net = xp_net(pols[0]);
	struct xfrm_state *xfrm[XFRM_MAX_DEPTH];
	struct xfrm_dst *bundle[XFRM_MAX_DEPTH];
	struct xfrm_dst *xdst;
	struct dst_entry *dst;
	int err;

	/* Try to instantiate a bundle */
	err = xfrm_tmpl_resolve(pols, num_pols, fl, xfrm, family);
	if (err <= 0) {
		if (err == 0)
			return NULL;

		if (err != -EAGAIN)
			XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTPOLERROR);
		return ERR_PTR(err);
	}

	dst = xfrm_bundle_create(pols[0], xfrm, bundle, err, fl, dst_orig);
	if (IS_ERR(dst)) {
		XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTBUNDLEGENERROR);
		return ERR_CAST(dst);
	}

	xdst = (struct xfrm_dst *)dst;
	xdst->num_xfrms = err;
	xdst->num_pols = num_pols;
	memcpy(xdst->pols, pols, sizeof(struct xfrm_policy *) * num_pols);
	xdst->policy_genid = atomic_read(&pols[0]->genid);

	return xdst;
}