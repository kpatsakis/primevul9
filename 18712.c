static void xfrm_init_pmtu(struct xfrm_dst **bundle, int nr)
{
	while (nr--) {
		struct xfrm_dst *xdst = bundle[nr];
		u32 pmtu, route_mtu_cached;
		struct dst_entry *dst;

		dst = &xdst->u.dst;
		pmtu = dst_mtu(xfrm_dst_child(dst));
		xdst->child_mtu_cached = pmtu;

		pmtu = xfrm_state_mtu(dst->xfrm, pmtu);

		route_mtu_cached = dst_mtu(xdst->route);
		xdst->route_mtu_cached = route_mtu_cached;

		if (pmtu > route_mtu_cached)
			pmtu = route_mtu_cached;

		dst_metric_set(dst, RTAX_MTU, pmtu);
	}
}