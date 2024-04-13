static void bnx2x_update_coalesce_sb(struct bnx2x *bp, u8 fw_sb_id,
				     u16 tx_usec, u16 rx_usec)
{
	bnx2x_update_coalesce_sb_index(bp, fw_sb_id, HC_INDEX_ETH_RX_CQ_CONS,
				    false, rx_usec);
	bnx2x_update_coalesce_sb_index(bp, fw_sb_id,
				       HC_INDEX_ETH_TX_CQ_CONS_COS0, false,
				       tx_usec);
	bnx2x_update_coalesce_sb_index(bp, fw_sb_id,
				       HC_INDEX_ETH_TX_CQ_CONS_COS1, false,
				       tx_usec);
	bnx2x_update_coalesce_sb_index(bp, fw_sb_id,
				       HC_INDEX_ETH_TX_CQ_CONS_COS2, false,
				       tx_usec);
}