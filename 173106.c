void bnx2x_tx_hw_flushed(struct bnx2x *bp, u32 poll_count)
{
	struct pbf_pN_cmd_regs cmd_regs[] = {
		{0, (CHIP_IS_E3B0(bp)) ?
			PBF_REG_TQ_OCCUPANCY_Q0 :
			PBF_REG_P0_TQ_OCCUPANCY,
		    (CHIP_IS_E3B0(bp)) ?
			PBF_REG_TQ_LINES_FREED_CNT_Q0 :
			PBF_REG_P0_TQ_LINES_FREED_CNT},
		{1, (CHIP_IS_E3B0(bp)) ?
			PBF_REG_TQ_OCCUPANCY_Q1 :
			PBF_REG_P1_TQ_OCCUPANCY,
		    (CHIP_IS_E3B0(bp)) ?
			PBF_REG_TQ_LINES_FREED_CNT_Q1 :
			PBF_REG_P1_TQ_LINES_FREED_CNT},
		{4, (CHIP_IS_E3B0(bp)) ?
			PBF_REG_TQ_OCCUPANCY_LB_Q :
			PBF_REG_P4_TQ_OCCUPANCY,
		    (CHIP_IS_E3B0(bp)) ?
			PBF_REG_TQ_LINES_FREED_CNT_LB_Q :
			PBF_REG_P4_TQ_LINES_FREED_CNT}
	};

	struct pbf_pN_buf_regs buf_regs[] = {
		{0, (CHIP_IS_E3B0(bp)) ?
			PBF_REG_INIT_CRD_Q0 :
			PBF_REG_P0_INIT_CRD ,
		    (CHIP_IS_E3B0(bp)) ?
			PBF_REG_CREDIT_Q0 :
			PBF_REG_P0_CREDIT,
		    (CHIP_IS_E3B0(bp)) ?
			PBF_REG_INTERNAL_CRD_FREED_CNT_Q0 :
			PBF_REG_P0_INTERNAL_CRD_FREED_CNT},
		{1, (CHIP_IS_E3B0(bp)) ?
			PBF_REG_INIT_CRD_Q1 :
			PBF_REG_P1_INIT_CRD,
		    (CHIP_IS_E3B0(bp)) ?
			PBF_REG_CREDIT_Q1 :
			PBF_REG_P1_CREDIT,
		    (CHIP_IS_E3B0(bp)) ?
			PBF_REG_INTERNAL_CRD_FREED_CNT_Q1 :
			PBF_REG_P1_INTERNAL_CRD_FREED_CNT},
		{4, (CHIP_IS_E3B0(bp)) ?
			PBF_REG_INIT_CRD_LB_Q :
			PBF_REG_P4_INIT_CRD,
		    (CHIP_IS_E3B0(bp)) ?
			PBF_REG_CREDIT_LB_Q :
			PBF_REG_P4_CREDIT,
		    (CHIP_IS_E3B0(bp)) ?
			PBF_REG_INTERNAL_CRD_FREED_CNT_LB_Q :
			PBF_REG_P4_INTERNAL_CRD_FREED_CNT},
	};

	int i;

	/* Verify the command queues are flushed P0, P1, P4 */
	for (i = 0; i < ARRAY_SIZE(cmd_regs); i++)
		bnx2x_pbf_pN_cmd_flushed(bp, &cmd_regs[i], poll_count);

	/* Verify the transmission buffers are flushed P0, P1, P4 */
	for (i = 0; i < ARRAY_SIZE(buf_regs); i++)
		bnx2x_pbf_pN_buf_flushed(bp, &buf_regs[i], poll_count);
}