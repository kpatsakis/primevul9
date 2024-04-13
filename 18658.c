static int xfrm_bundle_ok(struct xfrm_dst *first)
{
	struct xfrm_dst *bundle[XFRM_MAX_DEPTH];
	struct dst_entry *dst = &first->u.dst;
	struct xfrm_dst *xdst;
	int start_from, nr;
	u32 mtu;

	if (!dst_check(xfrm_dst_path(dst), ((struct xfrm_dst *)dst)->path_cookie) ||
	    (dst->dev && !netif_running(dst->dev)))
		return 0;

	if (dst->flags & DST_XFRM_QUEUE)
		return 1;

	start_from = nr = 0;
	do {
		struct xfrm_dst *xdst = (struct xfrm_dst *)dst;

		if (dst->xfrm->km.state != XFRM_STATE_VALID)
			return 0;
		if (xdst->xfrm_genid != dst->xfrm->genid)
			return 0;
		if (xdst->num_pols > 0 &&
		    xdst->policy_genid != atomic_read(&xdst->pols[0]->genid))
			return 0;

		bundle[nr++] = xdst;

		mtu = dst_mtu(xfrm_dst_child(dst));
		if (xdst->child_mtu_cached != mtu) {
			start_from = nr;
			xdst->child_mtu_cached = mtu;
		}

		if (!dst_check(xdst->route, xdst->route_cookie))
			return 0;
		mtu = dst_mtu(xdst->route);
		if (xdst->route_mtu_cached != mtu) {
			start_from = nr;
			xdst->route_mtu_cached = mtu;
		}

		dst = xfrm_dst_child(dst);
	} while (dst->xfrm);

	if (likely(!start_from))
		return 1;

	xdst = bundle[start_from - 1];
	mtu = xdst->child_mtu_cached;
	while (start_from--) {
		dst = &xdst->u.dst;

		mtu = xfrm_state_mtu(dst->xfrm, mtu);
		if (mtu > xdst->route_mtu_cached)
			mtu = xdst->route_mtu_cached;
		dst_metric_set(dst, RTAX_MTU, mtu);
		if (!start_from)
			break;

		xdst = bundle[start_from - 1];
		xdst->child_mtu_cached = mtu;
	}

	return 1;
}