static int  bnx2x_cnic_handle_cfc_del(struct bnx2x *bp, u32 cid,
				      union event_ring_elem *elem)
{
	u8 err = elem->message.error;

	if (!bp->cnic_eth_dev.starting_cid  ||
	    (cid < bp->cnic_eth_dev.starting_cid &&
	    cid != bp->cnic_eth_dev.iscsi_l2_cid))
		return 1;

	DP(BNX2X_MSG_SP, "got delete ramrod for CNIC CID %d\n", cid);

	if (unlikely(err)) {

		BNX2X_ERR("got delete ramrod for CNIC CID %d with error!\n",
			  cid);
		bnx2x_panic_dump(bp, false);
	}
	bnx2x_cnic_cfc_comp(bp, cid, err);
	return 0;
}