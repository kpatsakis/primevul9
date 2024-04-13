void bnx2x_nic_init_cnic(struct bnx2x *bp)
{
	if (!NO_FCOE(bp))
		bnx2x_init_fcoe_fp(bp);

	bnx2x_init_sb(bp, bp->cnic_sb_mapping,
		      BNX2X_VF_ID_INVALID, false,
		      bnx2x_cnic_fw_sb_id(bp), bnx2x_cnic_igu_sb_id(bp));

	/* ensure status block indices were read */
	rmb();
	bnx2x_init_rx_rings_cnic(bp);
	bnx2x_init_tx_rings_cnic(bp);

	/* flush all */
	mb();
	mmiowb();
}