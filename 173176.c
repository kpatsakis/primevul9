static void bnx2x_pf_tx_q_prep(struct bnx2x *bp,
	struct bnx2x_fastpath *fp, struct bnx2x_txq_setup_params *txq_init,
	u8 cos)
{
	txq_init->dscr_map = fp->txdata_ptr[cos]->tx_desc_mapping;
	txq_init->sb_cq_index = HC_INDEX_ETH_FIRST_TX_CQ_CONS + cos;
	txq_init->traffic_type = LLFC_TRAFFIC_TYPE_NW;
	txq_init->fw_sb_id = fp->fw_sb_id;

	/*
	 * set the tss leading client id for TX classification ==
	 * leading RSS client id
	 */
	txq_init->tss_leading_cl_id = bnx2x_fp(bp, 0, cl_id);

	if (IS_FCOE_FP(fp)) {
		txq_init->sb_cq_index = HC_SP_INDEX_ETH_FCOE_TX_CQ_CONS;
		txq_init->traffic_type = LLFC_TRAFFIC_TYPE_FCOE;
	}
}