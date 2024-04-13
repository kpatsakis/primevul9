static int bnx2x_cnic_ctl_send_bh(struct bnx2x *bp, struct cnic_ctl_info *ctl)
{
	struct cnic_ops *c_ops;
	int rc = 0;

	rcu_read_lock();
	c_ops = rcu_dereference(bp->cnic_ops);
	if (c_ops)
		rc = c_ops->cnic_ctl(bp->cnic_data, ctl);
	rcu_read_unlock();

	return rc;
}