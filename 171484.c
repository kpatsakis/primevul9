static int ql_is_petbi_neg_pause(struct ql3_adapter *qdev)
{
	u16 reg;

	if (ql_mii_read_reg(qdev, PETBI_NEG_PARTNER, &reg) < 0)
		return 0;

	return (reg & PETBI_NEG_PAUSE_MASK) == PETBI_NEG_PAUSE;
}