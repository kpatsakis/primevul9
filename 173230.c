static int bnx2x_er_poll_igu_vq(struct bnx2x *bp)
{
	u32 cnt = 1000;
	u32 pend_bits = 0;

	do {
		pend_bits  = REG_RD(bp, IGU_REG_PENDING_BITS_STATUS);

		if (pend_bits == 0)
			break;

		usleep_range(1000, 2000);
	} while (cnt-- > 0);

	if (cnt <= 0) {
		BNX2X_ERR("Still pending IGU requests pend_bits=%x!\n",
			  pend_bits);
		return -EBUSY;
	}

	return 0;
}