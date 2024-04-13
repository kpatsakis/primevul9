static void ql_phy_start_neg_ex(struct ql3_adapter *qdev)
{
	u16 reg;
	u16 portConfiguration;

	if (qdev->phyType == PHY_AGERE_ET1011C)
		ql_mii_write_reg(qdev, 0x13, 0x0000);
					/* turn off external loopback */

	if (qdev->mac_index == 0)
		portConfiguration =
			qdev->nvram_data.macCfg_port0.portConfiguration;
	else
		portConfiguration =
			qdev->nvram_data.macCfg_port1.portConfiguration;

	/*  Some HBA's in the field are set to 0 and they need to
	    be reinterpreted with a default value */
	if (portConfiguration == 0)
		portConfiguration = PORT_CONFIG_DEFAULT;

	/* Set the 1000 advertisements */
	ql_mii_read_reg_ex(qdev, PHY_GIG_CONTROL, &reg,
			   PHYAddr[qdev->mac_index]);
	reg &= ~PHY_GIG_ALL_PARAMS;

	if (portConfiguration & PORT_CONFIG_1000MB_SPEED) {
		if (portConfiguration & PORT_CONFIG_FULL_DUPLEX_ENABLED)
			reg |= PHY_GIG_ADV_1000F;
		else
			reg |= PHY_GIG_ADV_1000H;
	}

	ql_mii_write_reg_ex(qdev, PHY_GIG_CONTROL, reg,
			    PHYAddr[qdev->mac_index]);

	/* Set the 10/100 & pause negotiation advertisements */
	ql_mii_read_reg_ex(qdev, PHY_NEG_ADVER, &reg,
			   PHYAddr[qdev->mac_index]);
	reg &= ~PHY_NEG_ALL_PARAMS;

	if (portConfiguration & PORT_CONFIG_SYM_PAUSE_ENABLED)
		reg |= PHY_NEG_ASY_PAUSE | PHY_NEG_SYM_PAUSE;

	if (portConfiguration & PORT_CONFIG_FULL_DUPLEX_ENABLED) {
		if (portConfiguration & PORT_CONFIG_100MB_SPEED)
			reg |= PHY_NEG_ADV_100F;

		if (portConfiguration & PORT_CONFIG_10MB_SPEED)
			reg |= PHY_NEG_ADV_10F;
	}

	if (portConfiguration & PORT_CONFIG_HALF_DUPLEX_ENABLED) {
		if (portConfiguration & PORT_CONFIG_100MB_SPEED)
			reg |= PHY_NEG_ADV_100H;

		if (portConfiguration & PORT_CONFIG_10MB_SPEED)
			reg |= PHY_NEG_ADV_10H;
	}

	if (portConfiguration & PORT_CONFIG_1000MB_SPEED)
		reg |= 1;

	ql_mii_write_reg_ex(qdev, PHY_NEG_ADVER, reg,
			    PHYAddr[qdev->mac_index]);

	ql_mii_read_reg_ex(qdev, CONTROL_REG, &reg, PHYAddr[qdev->mac_index]);

	ql_mii_write_reg_ex(qdev, CONTROL_REG,
			    reg | PHY_CTRL_RESTART_NEG | PHY_CTRL_AUTO_NEG,
			    PHYAddr[qdev->mac_index]);
}