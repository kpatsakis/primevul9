static int ql_is_phy_neg_pause(struct ql3_adapter *qdev)
{
	u16 reg;

	if (ql_mii_read_reg(qdev, PHY_NEG_PARTNER, &reg) < 0)
		return 0;

	return (reg & PHY_NEG_PAUSE) != 0;
}