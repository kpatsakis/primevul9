struct dst_entry *xfrm_lookup_with_ifid(struct net *net,
					struct dst_entry *dst_orig,
					const struct flowi *fl,
					const struct sock *sk,
					int flags, u32 if_id)
{
	struct xfrm_policy *pols[XFRM_POLICY_TYPE_MAX];
	struct xfrm_dst *xdst;
	struct dst_entry *dst, *route;
	u16 family = dst_orig->ops->family;
	u8 dir = XFRM_POLICY_OUT;
	int i, err, num_pols, num_xfrms = 0, drop_pols = 0;

	dst = NULL;
	xdst = NULL;
	route = NULL;

	sk = sk_const_to_full_sk(sk);
	if (sk && sk->sk_policy[XFRM_POLICY_OUT]) {
		num_pols = 1;
		pols[0] = xfrm_sk_policy_lookup(sk, XFRM_POLICY_OUT, fl, family,
						if_id);
		err = xfrm_expand_policies(fl, family, pols,
					   &num_pols, &num_xfrms);
		if (err < 0)
			goto dropdst;

		if (num_pols) {
			if (num_xfrms <= 0) {
				drop_pols = num_pols;
				goto no_transform;
			}

			xdst = xfrm_resolve_and_create_bundle(
					pols, num_pols, fl,
					family, dst_orig);

			if (IS_ERR(xdst)) {
				xfrm_pols_put(pols, num_pols);
				err = PTR_ERR(xdst);
				if (err == -EREMOTE)
					goto nopol;

				goto dropdst;
			} else if (xdst == NULL) {
				num_xfrms = 0;
				drop_pols = num_pols;
				goto no_transform;
			}

			route = xdst->route;
		}
	}

	if (xdst == NULL) {
		struct xfrm_flo xflo;

		xflo.dst_orig = dst_orig;
		xflo.flags = flags;

		/* To accelerate a bit...  */
		if (!if_id && ((dst_orig->flags & DST_NOXFRM) ||
			       !net->xfrm.policy_count[XFRM_POLICY_OUT]))
			goto nopol;

		xdst = xfrm_bundle_lookup(net, fl, family, dir, &xflo, if_id);
		if (xdst == NULL)
			goto nopol;
		if (IS_ERR(xdst)) {
			err = PTR_ERR(xdst);
			goto dropdst;
		}

		num_pols = xdst->num_pols;
		num_xfrms = xdst->num_xfrms;
		memcpy(pols, xdst->pols, sizeof(struct xfrm_policy *) * num_pols);
		route = xdst->route;
	}

	dst = &xdst->u.dst;
	if (route == NULL && num_xfrms > 0) {
		/* The only case when xfrm_bundle_lookup() returns a
		 * bundle with null route, is when the template could
		 * not be resolved. It means policies are there, but
		 * bundle could not be created, since we don't yet
		 * have the xfrm_state's. We need to wait for KM to
		 * negotiate new SA's or bail out with error.*/
		if (net->xfrm.sysctl_larval_drop) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTNOSTATES);
			err = -EREMOTE;
			goto error;
		}

		err = -EAGAIN;

		XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTNOSTATES);
		goto error;
	}

no_transform:
	if (num_pols == 0)
		goto nopol;

	if ((flags & XFRM_LOOKUP_ICMP) &&
	    !(pols[0]->flags & XFRM_POLICY_ICMP)) {
		err = -ENOENT;
		goto error;
	}

	for (i = 0; i < num_pols; i++)
		pols[i]->curlft.use_time = ktime_get_real_seconds();

	if (num_xfrms < 0) {
		/* Prohibit the flow */
		XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTPOLBLOCK);
		err = -EPERM;
		goto error;
	} else if (num_xfrms > 0) {
		/* Flow transformed */
		dst_release(dst_orig);
	} else {
		/* Flow passes untransformed */
		dst_release(dst);
		dst = dst_orig;
	}
ok:
	xfrm_pols_put(pols, drop_pols);
	if (dst && dst->xfrm &&
	    dst->xfrm->props.mode == XFRM_MODE_TUNNEL)
		dst->flags |= DST_XFRM_TUNNEL;
	return dst;

nopol:
	if (!(dst_orig->dev->flags & IFF_LOOPBACK) &&
	    net->xfrm.policy_default[dir] == XFRM_USERPOLICY_BLOCK) {
		err = -EPERM;
		goto error;
	}
	if (!(flags & XFRM_LOOKUP_ICMP)) {
		dst = dst_orig;
		goto ok;
	}
	err = -ENOENT;
error:
	dst_release(dst);
dropdst:
	if (!(flags & XFRM_LOOKUP_KEEP_DST_REF))
		dst_release(dst_orig);
	xfrm_pols_put(pols, drop_pols);
	return ERR_PTR(err);
}