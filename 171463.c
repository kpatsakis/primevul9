static int ql_is_full_dup(struct ql3_adapter *qdev)
{
	u16 reg;

	switch (qdev->phyType) {
	case PHY_AGERE_ET1011C: {
		if (ql_mii_read_reg(qdev, 0x1A, &reg))
			return 0;

		return ((reg & 0x0080) && (reg & 0x1000)) != 0;
	}
	case PHY_VITESSE_VSC8211:
	default: {
		if (ql_mii_read_reg(qdev, AUX_CONTROL_STATUS, &reg) < 0)
			return 0;
		return (reg & PHY_AUX_DUPLEX_STAT) != 0;
	}
	}
}