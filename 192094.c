static int mce_no_way_out(struct mce *m, char **msg, unsigned long *validp,
			  struct pt_regs *regs)
{
	int i, ret = 0;
	char *tmp;

	for (i = 0; i < mca_cfg.banks; i++) {
		m->status = mce_rdmsrl(msr_ops.status(i));
		if (m->status & MCI_STATUS_VAL) {
			__set_bit(i, validp);
			if (quirk_no_way_out)
				quirk_no_way_out(i, m, regs);
		}

		if (mce_severity(m, mca_cfg.tolerant, &tmp, true) >= MCE_PANIC_SEVERITY) {
			*msg = tmp;
			ret = 1;
		}
	}
	return ret;
}