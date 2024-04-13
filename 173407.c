static void bnx2x_cnic_sp_post(struct bnx2x *bp, int count)
{
	struct eth_spe *spe;
	int cxt_index, cxt_offset;

#ifdef BNX2X_STOP_ON_ERROR
	if (unlikely(bp->panic))
		return;
#endif

	spin_lock_bh(&bp->spq_lock);
	BUG_ON(bp->cnic_spq_pending < count);
	bp->cnic_spq_pending -= count;

	for (; bp->cnic_kwq_pending; bp->cnic_kwq_pending--) {
		u16 type =  (le16_to_cpu(bp->cnic_kwq_cons->hdr.type)
				& SPE_HDR_CONN_TYPE) >>
				SPE_HDR_CONN_TYPE_SHIFT;
		u8 cmd = (le32_to_cpu(bp->cnic_kwq_cons->hdr.conn_and_cmd_data)
				>> SPE_HDR_CMD_ID_SHIFT) & 0xff;

		/* Set validation for iSCSI L2 client before sending SETUP
		 *  ramrod
		 */
		if (type == ETH_CONNECTION_TYPE) {
			if (cmd == RAMROD_CMD_ID_ETH_CLIENT_SETUP) {
				cxt_index = BNX2X_ISCSI_ETH_CID(bp) /
					ILT_PAGE_CIDS;
				cxt_offset = BNX2X_ISCSI_ETH_CID(bp) -
					(cxt_index * ILT_PAGE_CIDS);
				bnx2x_set_ctx_validation(bp,
					&bp->context[cxt_index].
							 vcxt[cxt_offset].eth,
					BNX2X_ISCSI_ETH_CID(bp));
			}
		}

		/*
		 * There may be not more than 8 L2, not more than 8 L5 SPEs
		 * and in the air. We also check that number of outstanding
		 * COMMON ramrods is not more than the EQ and SPQ can
		 * accommodate.
		 */
		if (type == ETH_CONNECTION_TYPE) {
			if (!atomic_read(&bp->cq_spq_left))
				break;
			else
				atomic_dec(&bp->cq_spq_left);
		} else if (type == NONE_CONNECTION_TYPE) {
			if (!atomic_read(&bp->eq_spq_left))
				break;
			else
				atomic_dec(&bp->eq_spq_left);
		} else if ((type == ISCSI_CONNECTION_TYPE) ||
			   (type == FCOE_CONNECTION_TYPE)) {
			if (bp->cnic_spq_pending >=
			    bp->cnic_eth_dev.max_kwqe_pending)
				break;
			else
				bp->cnic_spq_pending++;
		} else {
			BNX2X_ERR("Unknown SPE type: %d\n", type);
			bnx2x_panic();
			break;
		}

		spe = bnx2x_sp_get_next(bp);
		*spe = *bp->cnic_kwq_cons;

		DP(BNX2X_MSG_SP, "pending on SPQ %d, on KWQ %d count %d\n",
		   bp->cnic_spq_pending, bp->cnic_kwq_pending, count);

		if (bp->cnic_kwq_cons == bp->cnic_kwq_last)
			bp->cnic_kwq_cons = bp->cnic_kwq;
		else
			bp->cnic_kwq_cons++;
	}
	bnx2x_sp_prod_update(bp);
	spin_unlock_bh(&bp->spq_lock);
}