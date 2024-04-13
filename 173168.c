void bnx2x_post_irq_nic_init(struct bnx2x *bp, u32 load_code)
{
	bnx2x_init_eq_ring(bp);
	bnx2x_init_internal(bp, load_code);
	bnx2x_pf_init(bp);
	bnx2x_stats_init(bp);

	/* flush all before enabling interrupts */
	mb();
	mmiowb();

	bnx2x_int_enable(bp);

	/* Check for SPIO5 */
	bnx2x_attn_int_deasserted0(bp,
		REG_RD(bp, MISC_REG_AEU_AFTER_INVERT_1_FUNC_0 + BP_PORT(bp)*4) &
				   AEU_INPUTS_ATTN_BITS_SPIO5);
}