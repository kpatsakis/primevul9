static struct xfrm_dst *xfrm_create_dummy_bundle(struct net *net,
						 struct xfrm_flo *xflo,
						 const struct flowi *fl,
						 int num_xfrms,
						 u16 family)
{
	int err;
	struct net_device *dev;
	struct dst_entry *dst;
	struct dst_entry *dst1;
	struct xfrm_dst *xdst;

	xdst = xfrm_alloc_dst(net, family);
	if (IS_ERR(xdst))
		return xdst;

	if (!(xflo->flags & XFRM_LOOKUP_QUEUE) ||
	    net->xfrm.sysctl_larval_drop ||
	    num_xfrms <= 0)
		return xdst;

	dst = xflo->dst_orig;
	dst1 = &xdst->u.dst;
	dst_hold(dst);
	xdst->route = dst;

	dst_copy_metrics(dst1, dst);

	dst1->obsolete = DST_OBSOLETE_FORCE_CHK;
	dst1->flags |= DST_XFRM_QUEUE;
	dst1->lastuse = jiffies;

	dst1->input = dst_discard;
	dst1->output = xdst_queue_output;

	dst_hold(dst);
	xfrm_dst_set_child(xdst, dst);
	xdst->path = dst;

	xfrm_init_path((struct xfrm_dst *)dst1, dst, 0);

	err = -ENODEV;
	dev = dst->dev;
	if (!dev)
		goto free_dst;

	err = xfrm_fill_dst(xdst, dev, fl);
	if (err)
		goto free_dst;

out:
	return xdst;

free_dst:
	dst_release(dst1);
	xdst = ERR_PTR(err);
	goto out;
}