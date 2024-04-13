xfrm_tmpl_resolve_one(struct xfrm_policy *policy, const struct flowi *fl,
		      struct xfrm_state **xfrm, unsigned short family)
{
	struct net *net = xp_net(policy);
	int nx;
	int i, error;
	xfrm_address_t *daddr = xfrm_flowi_daddr(fl, family);
	xfrm_address_t *saddr = xfrm_flowi_saddr(fl, family);
	xfrm_address_t tmp;

	for (nx = 0, i = 0; i < policy->xfrm_nr; i++) {
		struct xfrm_state *x;
		xfrm_address_t *remote = daddr;
		xfrm_address_t *local  = saddr;
		struct xfrm_tmpl *tmpl = &policy->xfrm_vec[i];

		if (tmpl->mode == XFRM_MODE_TUNNEL ||
		    tmpl->mode == XFRM_MODE_BEET) {
			remote = &tmpl->id.daddr;
			local = &tmpl->saddr;
			if (xfrm_addr_any(local, tmpl->encap_family)) {
				error = xfrm_get_saddr(net, fl->flowi_oif,
						       &tmp, remote,
						       tmpl->encap_family, 0);
				if (error)
					goto fail;
				local = &tmp;
			}
		}

		x = xfrm_state_find(remote, local, fl, tmpl, policy, &error,
				    family, policy->if_id);

		if (x && x->km.state == XFRM_STATE_VALID) {
			xfrm[nx++] = x;
			daddr = remote;
			saddr = local;
			continue;
		}
		if (x) {
			error = (x->km.state == XFRM_STATE_ERROR ?
				 -EINVAL : -EAGAIN);
			xfrm_state_put(x);
		} else if (error == -ESRCH) {
			error = -EAGAIN;
		}

		if (!tmpl->optional)
			goto fail;
	}
	return nx;

fail:
	for (nx--; nx >= 0; nx--)
		xfrm_state_put(xfrm[nx]);
	return error;
}