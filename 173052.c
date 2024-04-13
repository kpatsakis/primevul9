void bnx2x_init_sb(struct bnx2x *bp, dma_addr_t mapping, int vfid,
			  u8 vf_valid, int fw_sb_id, int igu_sb_id)
{
	int igu_seg_id;

	struct hc_status_block_data_e2 sb_data_e2;
	struct hc_status_block_data_e1x sb_data_e1x;
	struct hc_status_block_sm  *hc_sm_p;
	int data_size;
	u32 *sb_data_p;

	if (CHIP_INT_MODE_IS_BC(bp))
		igu_seg_id = HC_SEG_ACCESS_NORM;
	else
		igu_seg_id = IGU_SEG_ACCESS_NORM;

	bnx2x_zero_fp_sb(bp, fw_sb_id);

	if (!CHIP_IS_E1x(bp)) {
		memset(&sb_data_e2, 0, sizeof(struct hc_status_block_data_e2));
		sb_data_e2.common.state = SB_ENABLED;
		sb_data_e2.common.p_func.pf_id = BP_FUNC(bp);
		sb_data_e2.common.p_func.vf_id = vfid;
		sb_data_e2.common.p_func.vf_valid = vf_valid;
		sb_data_e2.common.p_func.vnic_id = BP_VN(bp);
		sb_data_e2.common.same_igu_sb_1b = true;
		sb_data_e2.common.host_sb_addr.hi = U64_HI(mapping);
		sb_data_e2.common.host_sb_addr.lo = U64_LO(mapping);
		hc_sm_p = sb_data_e2.common.state_machine;
		sb_data_p = (u32 *)&sb_data_e2;
		data_size = sizeof(struct hc_status_block_data_e2)/sizeof(u32);
		bnx2x_map_sb_state_machines(sb_data_e2.index_data);
	} else {
		memset(&sb_data_e1x, 0,
		       sizeof(struct hc_status_block_data_e1x));
		sb_data_e1x.common.state = SB_ENABLED;
		sb_data_e1x.common.p_func.pf_id = BP_FUNC(bp);
		sb_data_e1x.common.p_func.vf_id = 0xff;
		sb_data_e1x.common.p_func.vf_valid = false;
		sb_data_e1x.common.p_func.vnic_id = BP_VN(bp);
		sb_data_e1x.common.same_igu_sb_1b = true;
		sb_data_e1x.common.host_sb_addr.hi = U64_HI(mapping);
		sb_data_e1x.common.host_sb_addr.lo = U64_LO(mapping);
		hc_sm_p = sb_data_e1x.common.state_machine;
		sb_data_p = (u32 *)&sb_data_e1x;
		data_size = sizeof(struct hc_status_block_data_e1x)/sizeof(u32);
		bnx2x_map_sb_state_machines(sb_data_e1x.index_data);
	}

	bnx2x_setup_ndsb_state_machine(&hc_sm_p[SM_RX_ID],
				       igu_sb_id, igu_seg_id);
	bnx2x_setup_ndsb_state_machine(&hc_sm_p[SM_TX_ID],
				       igu_sb_id, igu_seg_id);

	DP(NETIF_MSG_IFUP, "Init FW SB %d\n", fw_sb_id);

	/* write indices to HW - PCI guarantees endianity of regpairs */
	bnx2x_wr_fp_sb_data(bp, fw_sb_id, sb_data_p, data_size);
}