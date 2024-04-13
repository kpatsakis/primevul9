void xfrm_if_register_cb(const struct xfrm_if_cb *ifcb)
{
	spin_lock(&xfrm_if_cb_lock);
	rcu_assign_pointer(xfrm_if_cb, ifcb);
	spin_unlock(&xfrm_if_cb_lock);
}