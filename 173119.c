static void bnx2x_attn_int_deasserted2(struct bnx2x *bp, u32 attn)
{
	u32 val;

	if (attn & AEU_INPUTS_ATTN_BITS_CFC_HW_INTERRUPT) {

		val = REG_RD(bp, CFC_REG_CFC_INT_STS_CLR);
		BNX2X_ERR("CFC hw attention 0x%x\n", val);
		/* CFC error attention */
		if (val & 0x2)
			BNX2X_ERR("FATAL error from CFC\n");
	}

	if (attn & AEU_INPUTS_ATTN_BITS_PXP_HW_INTERRUPT) {
		val = REG_RD(bp, PXP_REG_PXP_INT_STS_CLR_0);
		BNX2X_ERR("PXP hw attention-0 0x%x\n", val);
		/* RQ_USDMDP_FIFO_OVERFLOW */
		if (val & 0x18000)
			BNX2X_ERR("FATAL error from PXP\n");

		if (!CHIP_IS_E1x(bp)) {
			val = REG_RD(bp, PXP_REG_PXP_INT_STS_CLR_1);
			BNX2X_ERR("PXP hw attention-1 0x%x\n", val);
		}
	}

	if (attn & HW_INTERRUPT_ASSERT_SET_2) {

		int port = BP_PORT(bp);
		int reg_offset;

		reg_offset = (port ? MISC_REG_AEU_ENABLE1_FUNC_1_OUT_2 :
				     MISC_REG_AEU_ENABLE1_FUNC_0_OUT_2);

		val = REG_RD(bp, reg_offset);
		val &= ~(attn & HW_INTERRUPT_ASSERT_SET_2);
		REG_WR(bp, reg_offset, val);

		BNX2X_ERR("FATAL HW block attention set2 0x%x\n",
			  (u32)(attn & HW_INTERRUPT_ASSERT_SET_2));
		bnx2x_panic();
	}
}