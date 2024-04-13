static unsigned long bnx2x_get_q_flags(struct bnx2x *bp,
				       struct bnx2x_fastpath *fp,
				       bool leading)
{
	unsigned long flags = 0;

	/* calculate other queue flags */
	if (IS_MF_SD(bp))
		__set_bit(BNX2X_Q_FLG_OV, &flags);

	if (IS_FCOE_FP(fp)) {
		__set_bit(BNX2X_Q_FLG_FCOE, &flags);
		/* For FCoE - force usage of default priority (for afex) */
		__set_bit(BNX2X_Q_FLG_FORCE_DEFAULT_PRI, &flags);
	}

	if (fp->mode != TPA_MODE_DISABLED) {
		__set_bit(BNX2X_Q_FLG_TPA, &flags);
		__set_bit(BNX2X_Q_FLG_TPA_IPV6, &flags);
		if (fp->mode == TPA_MODE_GRO)
			__set_bit(BNX2X_Q_FLG_TPA_GRO, &flags);
	}

	if (leading) {
		__set_bit(BNX2X_Q_FLG_LEADING_RSS, &flags);
		__set_bit(BNX2X_Q_FLG_MCAST, &flags);
	}

	/* Always set HW VLAN stripping */
	__set_bit(BNX2X_Q_FLG_VLAN, &flags);

	/* configure silent vlan removal */
	if (IS_MF_AFEX(bp))
		__set_bit(BNX2X_Q_FLG_SILENT_VLAN_REM, &flags);

	return flags | bnx2x_get_common_flags(bp, fp, true);
}