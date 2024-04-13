int bnx2x_cnic_notify(struct bnx2x *bp, int cmd)
{
	struct cnic_ctl_info ctl = {0};

	ctl.cmd = cmd;

	return bnx2x_cnic_ctl_send(bp, &ctl);
}