int bnx2x_acquire_hw_lock(struct bnx2x *bp, u32 resource)
{
	u32 lock_status;
	u32 resource_bit = (1 << resource);
	int func = BP_FUNC(bp);
	u32 hw_lock_control_reg;
	int cnt;

	/* Validating that the resource is within range */
	if (resource > HW_LOCK_MAX_RESOURCE_VALUE) {
		BNX2X_ERR("resource(0x%x) > HW_LOCK_MAX_RESOURCE_VALUE(0x%x)\n",
		   resource, HW_LOCK_MAX_RESOURCE_VALUE);
		return -EINVAL;
	}

	if (func <= 5) {
		hw_lock_control_reg = (MISC_REG_DRIVER_CONTROL_1 + func*8);
	} else {
		hw_lock_control_reg =
				(MISC_REG_DRIVER_CONTROL_7 + (func - 6)*8);
	}

	/* Validating that the resource is not already taken */
	lock_status = REG_RD(bp, hw_lock_control_reg);
	if (lock_status & resource_bit) {
		BNX2X_ERR("lock_status 0x%x  resource_bit 0x%x\n",
		   lock_status, resource_bit);
		return -EEXIST;
	}

	/* Try for 5 second every 5ms */
	for (cnt = 0; cnt < 1000; cnt++) {
		/* Try to acquire the lock */
		REG_WR(bp, hw_lock_control_reg + 4, resource_bit);
		lock_status = REG_RD(bp, hw_lock_control_reg);
		if (lock_status & resource_bit)
			return 0;

		usleep_range(5000, 10000);
	}
	BNX2X_ERR("Timeout\n");
	return -EAGAIN;
}