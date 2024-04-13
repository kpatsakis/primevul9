int bnx2x_set_mult_gpio(struct bnx2x *bp, u8 pins, u32 mode)
{
	u32 gpio_reg = 0;
	int rc = 0;

	/* Any port swapping should be handled by caller. */

	bnx2x_acquire_hw_lock(bp, HW_LOCK_RESOURCE_GPIO);
	/* read GPIO and mask except the float bits */
	gpio_reg = REG_RD(bp, MISC_REG_GPIO);
	gpio_reg &= ~(pins << MISC_REGISTERS_GPIO_FLOAT_POS);
	gpio_reg &= ~(pins << MISC_REGISTERS_GPIO_CLR_POS);
	gpio_reg &= ~(pins << MISC_REGISTERS_GPIO_SET_POS);

	switch (mode) {
	case MISC_REGISTERS_GPIO_OUTPUT_LOW:
		DP(NETIF_MSG_LINK, "Set GPIO 0x%x -> output low\n", pins);
		/* set CLR */
		gpio_reg |= (pins << MISC_REGISTERS_GPIO_CLR_POS);
		break;

	case MISC_REGISTERS_GPIO_OUTPUT_HIGH:
		DP(NETIF_MSG_LINK, "Set GPIO 0x%x -> output high\n", pins);
		/* set SET */
		gpio_reg |= (pins << MISC_REGISTERS_GPIO_SET_POS);
		break;

	case MISC_REGISTERS_GPIO_INPUT_HI_Z:
		DP(NETIF_MSG_LINK, "Set GPIO 0x%x -> input\n", pins);
		/* set FLOAT */
		gpio_reg |= (pins << MISC_REGISTERS_GPIO_FLOAT_POS);
		break;

	default:
		BNX2X_ERR("Invalid GPIO mode assignment %d\n", mode);
		rc = -EINVAL;
		break;
	}

	if (rc == 0)
		REG_WR(bp, MISC_REG_GPIO, gpio_reg);

	bnx2x_release_hw_lock(bp, HW_LOCK_RESOURCE_GPIO);

	return rc;
}