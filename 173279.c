int bnx2x_sp_post(struct bnx2x *bp, int command, int cid,
		  u32 data_hi, u32 data_lo, int cmd_type)
{
	struct eth_spe *spe;
	u16 type;
	bool common = bnx2x_is_contextless_ramrod(command, cmd_type);

#ifdef BNX2X_STOP_ON_ERROR
	if (unlikely(bp->panic)) {
		BNX2X_ERR("Can't post SP when there is panic\n");
		return -EIO;
	}
#endif

	spin_lock_bh(&bp->spq_lock);

	if (common) {
		if (!atomic_read(&bp->eq_spq_left)) {
			BNX2X_ERR("BUG! EQ ring full!\n");
			spin_unlock_bh(&bp->spq_lock);
			bnx2x_panic();
			return -EBUSY;
		}
	} else if (!atomic_read(&bp->cq_spq_left)) {
			BNX2X_ERR("BUG! SPQ ring full!\n");
			spin_unlock_bh(&bp->spq_lock);
			bnx2x_panic();
			return -EBUSY;
	}

	spe = bnx2x_sp_get_next(bp);

	/* CID needs port number to be encoded int it */
	spe->hdr.conn_and_cmd_data =
			cpu_to_le32((command << SPE_HDR_CMD_ID_SHIFT) |
				    HW_CID(bp, cid));

	/* In some cases, type may already contain the func-id
	 * mainly in SRIOV related use cases, so we add it here only
	 * if it's not already set.
	 */
	if (!(cmd_type & SPE_HDR_FUNCTION_ID)) {
		type = (cmd_type << SPE_HDR_CONN_TYPE_SHIFT) &
			SPE_HDR_CONN_TYPE;
		type |= ((BP_FUNC(bp) << SPE_HDR_FUNCTION_ID_SHIFT) &
			 SPE_HDR_FUNCTION_ID);
	} else {
		type = cmd_type;
	}

	spe->hdr.type = cpu_to_le16(type);

	spe->data.update_data_addr.hi = cpu_to_le32(data_hi);
	spe->data.update_data_addr.lo = cpu_to_le32(data_lo);

	/*
	 * It's ok if the actual decrement is issued towards the memory
	 * somewhere between the spin_lock and spin_unlock. Thus no
	 * more explicit memory barrier is needed.
	 */
	if (common)
		atomic_dec(&bp->eq_spq_left);
	else
		atomic_dec(&bp->cq_spq_left);

	DP(BNX2X_MSG_SP,
	   "SPQE[%x] (%x:%x)  (cmd, common?) (%d,%d)  hw_cid %x  data (%x:%x) type(0x%x) left (CQ, EQ) (%x,%x)\n",
	   bp->spq_prod_idx, (u32)U64_HI(bp->spq_mapping),
	   (u32)(U64_LO(bp->spq_mapping) +
	   (void *)bp->spq_prod_bd - (void *)bp->spq), command, common,
	   HW_CID(bp, cid), data_hi, data_lo, type,
	   atomic_read(&bp->cq_spq_left), atomic_read(&bp->eq_spq_left));

	bnx2x_sp_prod_update(bp);
	spin_unlock_bh(&bp->spq_lock);
	return 0;
}