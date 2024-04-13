static const struct xfrm_if_cb *xfrm_if_get_cb(void)
{
	return rcu_dereference(xfrm_if_cb);
}