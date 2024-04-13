static void bnx2x_setup_fan_failure_detection(struct bnx2x *bp)
{
	int is_required;
	u32 val;
	int port;

	if (BP_NOMCP(bp))
		return;

	is_required = 0;
	val = SHMEM_RD(bp, dev_info.shared_hw_config.config2) &
	      SHARED_HW_CFG_FAN_FAILURE_MASK;

	if (val == SHARED_HW_CFG_FAN_FAILURE_ENABLED)
		is_required = 1;

	/*
	 * The fan failure mechanism is usually related to the PHY type since
	 * the power consumption of the board is affected by the PHY. Currently,
	 * fan is required for most designs with SFX7101, BCM8727 and BCM8481.
	 */
	else if (val == SHARED_HW_CFG_FAN_FAILURE_PHY_TYPE)
		for (port = PORT_0; port < PORT_MAX; port++) {
			is_required |=
				bnx2x_fan_failure_det_req(
					bp,
					bp->common.shmem_base,
					bp->common.shmem2_base,
					port);
		}

	DP(NETIF_MSG_HW, "fan detection setting: %d\n", is_required);

	if (is_required == 0)
		return;

	/* Fan failure is indicated by SPIO 5 */
	bnx2x_set_spio(bp, MISC_SPIO_SPIO5, MISC_SPIO_INPUT_HI_Z);

	/* set to active low mode */
	val = REG_RD(bp, MISC_REG_SPIO_INT);
	val |= (MISC_SPIO_SPIO5 << MISC_SPIO_INT_OLD_SET_POS);
	REG_WR(bp, MISC_REG_SPIO_INT, val);

	/* enable interrupt to signal the IGU */
	val = REG_RD(bp, MISC_REG_SPIO_EVENT_EN);
	val |= MISC_SPIO_SPIO5;
	REG_WR(bp, MISC_REG_SPIO_EVENT_EN, val);
}