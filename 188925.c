void i40e_do_reset_safe(struct i40e_pf *pf, u32 reset_flags)
{
	rtnl_lock();
	i40e_do_reset(pf, reset_flags, true);
	rtnl_unlock();
}