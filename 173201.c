static int bnx2x_set_spio(struct bnx2x *bp, int spio, u32 mode)
{
	u32 spio_reg;

	/* Only 2 SPIOs are configurable */
	if ((spio != MISC_SPIO_SPIO4) && (spio != MISC_SPIO_SPIO5)) {
		BNX2X_ERR("Invalid SPIO 0x%x\n", spio);
		return -EINVAL;
	}

	bnx2x_acquire_hw_lock(bp, HW_LOCK_RESOURCE_SPIO);
	/* read SPIO and mask except the float bits */
	spio_reg = (REG_RD(bp, MISC_REG_SPIO) & MISC_SPIO_FLOAT);

	switch (mode) {
	case MISC_SPIO_OUTPUT_LOW:
		DP(NETIF_MSG_HW, "Set SPIO 0x%x -> output low\n", spio);
		/* clear FLOAT and set CLR */
		spio_reg &= ~(spio << MISC_SPIO_FLOAT_POS);
		spio_reg |=  (spio << MISC_SPIO_CLR_POS);
		break;

	case MISC_SPIO_OUTPUT_HIGH:
		DP(NETIF_MSG_HW, "Set SPIO 0x%x -> output high\n", spio);
		/* clear FLOAT and set SET */
		spio_reg &= ~(spio << MISC_SPIO_FLOAT_POS);
		spio_reg |=  (spio << MISC_SPIO_SET_POS);
		break;

	case MISC_SPIO_INPUT_HI_Z:
		DP(NETIF_MSG_HW, "Set SPIO 0x%x -> input\n", spio);
		/* set FLOAT */
		spio_reg |= (spio << MISC_SPIO_FLOAT_POS);
		break;

	default:
		break;
	}

	REG_WR(bp, MISC_REG_SPIO, spio_reg);
	bnx2x_release_hw_lock(bp, HW_LOCK_RESOURCE_SPIO);

	return 0;
}