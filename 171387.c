static int ql_phy_get_speed(struct ql3_adapter *qdev)
{
	u16 reg;

	switch (qdev->phyType) {
	case PHY_AGERE_ET1011C: {
		if (ql_mii_read_reg(qdev, 0x1A, &reg) < 0)
			return 0;

		reg = (reg >> 8) & 3;
		break;
	}
	default:
		if (ql_mii_read_reg(qdev, AUX_CONTROL_STATUS, &reg) < 0)
			return 0;

		reg = (((reg & 0x18) >> 3) & 3);
	}

	switch (reg) {
	case 2:
		return SPEED_1000;
	case 1:
		return SPEED_100;
	case 0:
		return SPEED_10;
	default:
		return -1;
	}
}