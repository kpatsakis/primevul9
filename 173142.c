static struct bnx2x_queue_sp_obj *bnx2x_cid_to_q_obj(
	struct bnx2x *bp, u32 cid)
{
	DP(BNX2X_MSG_SP, "retrieving fp from cid %d\n", cid);

	if (CNIC_LOADED(bp) && (cid == BNX2X_FCOE_ETH_CID(bp)))
		return &bnx2x_fcoe_sp_obj(bp, q_obj);
	else
		return &bp->sp_objs[CID_TO_FP(cid, bp)].q_obj;
}