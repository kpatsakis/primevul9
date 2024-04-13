static void mce_read_aux(struct mce *m, int i)
{
	if (m->status & MCI_STATUS_MISCV)
		m->misc = mce_rdmsrl(msr_ops.misc(i));

	if (m->status & MCI_STATUS_ADDRV) {
		m->addr = mce_rdmsrl(msr_ops.addr(i));

		/*
		 * Mask the reported address by the reported granularity.
		 */
		if (mca_cfg.ser && (m->status & MCI_STATUS_MISCV)) {
			u8 shift = MCI_MISC_ADDR_LSB(m->misc);
			m->addr >>= shift;
			m->addr <<= shift;
		}

		/*
		 * Extract [55:<lsb>] where lsb is the least significant
		 * *valid* bit of the address bits.
		 */
		if (mce_flags.smca) {
			u8 lsb = (m->addr >> 56) & 0x3f;

			m->addr &= GENMASK_ULL(55, lsb);
		}
	}

	if (mce_flags.smca) {
		m->ipid = mce_rdmsrl(MSR_AMD64_SMCA_MCx_IPID(i));

		if (m->status & MCI_STATUS_SYNDV)
			m->synd = mce_rdmsrl(MSR_AMD64_SMCA_MCx_SYND(i));
	}
}