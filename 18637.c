void xfrm_if_unregister_cb(void)
{
	RCU_INIT_POINTER(xfrm_if_cb, NULL);
	synchronize_rcu();
}