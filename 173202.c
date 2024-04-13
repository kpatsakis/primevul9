static void bnx2x_cnic_cfc_comp(struct bnx2x *bp, int cid, u8 err)
{
	struct cnic_ctl_info ctl = {0};

	/* first we tell CNIC and only then we count this as a completion */
	ctl.cmd = CNIC_CTL_COMPLETION_CMD;
	ctl.data.comp.cid = cid;
	ctl.data.comp.error = err;

	bnx2x_cnic_ctl_send_bh(bp, &ctl);
	bnx2x_cnic_sp_post(bp, 0);
}