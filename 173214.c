static int bnx2x_acquire_alr(struct bnx2x *bp)
{
	u32 j, val;
	int rc = 0;

	might_sleep();
	for (j = 0; j < 1000; j++) {
		REG_WR(bp, MCP_REG_MCPR_ACCESS_LOCK, MCPR_ACCESS_LOCK_LOCK);
		val = REG_RD(bp, MCP_REG_MCPR_ACCESS_LOCK);
		if (val & MCPR_ACCESS_LOCK_LOCK)
			break;

		usleep_range(5000, 10000);
	}
	if (!(val & MCPR_ACCESS_LOCK_LOCK)) {
		BNX2X_ERR("Cannot acquire MCP access lock register\n");
		rc = -EBUSY;
	}

	return rc;
}