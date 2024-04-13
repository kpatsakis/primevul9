static void bnx2x_ilt_wr(struct bnx2x *bp, u32 index, dma_addr_t addr)
{
	int reg;
	u32 wb_write[2];

	if (CHIP_IS_E1(bp))
		reg = PXP2_REG_RQ_ONCHIP_AT + index*8;
	else
		reg = PXP2_REG_RQ_ONCHIP_AT_B0 + index*8;

	wb_write[0] = ONCHIP_ADDR1(addr);
	wb_write[1] = ONCHIP_ADDR2(addr);
	REG_WR_DMAE(bp, reg, wb_write, 2);
}