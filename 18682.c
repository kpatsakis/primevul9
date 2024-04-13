static struct dst_entry *xfrm_bundle_create(struct xfrm_policy *policy,
					    struct xfrm_state **xfrm,
					    struct xfrm_dst **bundle,
					    int nx,
					    const struct flowi *fl,
					    struct dst_entry *dst)
{
	const struct xfrm_state_afinfo *afinfo;
	const struct xfrm_mode *inner_mode;
	struct net *net = xp_net(policy);
	unsigned long now = jiffies;
	struct net_device *dev;
	struct xfrm_dst *xdst_prev = NULL;
	struct xfrm_dst *xdst0 = NULL;
	int i = 0;
	int err;
	int header_len = 0;
	int nfheader_len = 0;
	int trailer_len = 0;
	int tos;
	int family = policy->selector.family;
	xfrm_address_t saddr, daddr;

	xfrm_flowi_addr_get(fl, &saddr, &daddr, family);

	tos = xfrm_get_tos(fl, family);

	dst_hold(dst);

	for (; i < nx; i++) {
		struct xfrm_dst *xdst = xfrm_alloc_dst(net, family);
		struct dst_entry *dst1 = &xdst->u.dst;

		err = PTR_ERR(xdst);
		if (IS_ERR(xdst)) {
			dst_release(dst);
			goto put_states;
		}

		bundle[i] = xdst;
		if (!xdst_prev)
			xdst0 = xdst;
		else
			/* Ref count is taken during xfrm_alloc_dst()
			 * No need to do dst_clone() on dst1
			 */
			xfrm_dst_set_child(xdst_prev, &xdst->u.dst);

		if (xfrm[i]->sel.family == AF_UNSPEC) {
			inner_mode = xfrm_ip2inner_mode(xfrm[i],
							xfrm_af2proto(family));
			if (!inner_mode) {
				err = -EAFNOSUPPORT;
				dst_release(dst);
				goto put_states;
			}
		} else
			inner_mode = &xfrm[i]->inner_mode;

		xdst->route = dst;
		dst_copy_metrics(dst1, dst);

		if (xfrm[i]->props.mode != XFRM_MODE_TRANSPORT) {
			__u32 mark = 0;
			int oif;

			if (xfrm[i]->props.smark.v || xfrm[i]->props.smark.m)
				mark = xfrm_smark_get(fl->flowi_mark, xfrm[i]);

			family = xfrm[i]->props.family;
			oif = fl->flowi_oif ? : fl->flowi_l3mdev;
			dst = xfrm_dst_lookup(xfrm[i], tos, oif,
					      &saddr, &daddr, family, mark);
			err = PTR_ERR(dst);
			if (IS_ERR(dst))
				goto put_states;
		} else
			dst_hold(dst);

		dst1->xfrm = xfrm[i];
		xdst->xfrm_genid = xfrm[i]->genid;

		dst1->obsolete = DST_OBSOLETE_FORCE_CHK;
		dst1->lastuse = now;

		dst1->input = dst_discard;

		rcu_read_lock();
		afinfo = xfrm_state_afinfo_get_rcu(inner_mode->family);
		if (likely(afinfo))
			dst1->output = afinfo->output;
		else
			dst1->output = dst_discard_out;
		rcu_read_unlock();

		xdst_prev = xdst;

		header_len += xfrm[i]->props.header_len;
		if (xfrm[i]->type->flags & XFRM_TYPE_NON_FRAGMENT)
			nfheader_len += xfrm[i]->props.header_len;
		trailer_len += xfrm[i]->props.trailer_len;
	}

	xfrm_dst_set_child(xdst_prev, dst);
	xdst0->path = dst;

	err = -ENODEV;
	dev = dst->dev;
	if (!dev)
		goto free_dst;

	xfrm_init_path(xdst0, dst, nfheader_len);
	xfrm_init_pmtu(bundle, nx);

	for (xdst_prev = xdst0; xdst_prev != (struct xfrm_dst *)dst;
	     xdst_prev = (struct xfrm_dst *) xfrm_dst_child(&xdst_prev->u.dst)) {
		err = xfrm_fill_dst(xdst_prev, dev, fl);
		if (err)
			goto free_dst;

		xdst_prev->u.dst.header_len = header_len;
		xdst_prev->u.dst.trailer_len = trailer_len;
		header_len -= xdst_prev->u.dst.xfrm->props.header_len;
		trailer_len -= xdst_prev->u.dst.xfrm->props.trailer_len;
	}

	return &xdst0->u.dst;

put_states:
	for (; i < nx; i++)
		xfrm_state_put(xfrm[i]);
free_dst:
	if (xdst0)
		dst_release_immediate(&xdst0->u.dst);

	return ERR_PTR(err);
}