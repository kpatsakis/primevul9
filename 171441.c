static void ql_petbi_start_neg_ex(struct ql3_adapter *qdev)
{
	u16 reg;

	/* Enable Auto-negotiation sense */
	ql_mii_read_reg_ex(qdev, PETBI_TBI_CTRL, &reg,
			   PHYAddr[qdev->mac_index]);
	reg |= PETBI_TBI_AUTO_SENSE;
	ql_mii_write_reg_ex(qdev, PETBI_TBI_CTRL, reg,
			    PHYAddr[qdev->mac_index]);

	ql_mii_write_reg_ex(qdev, PETBI_NEG_ADVER,
			    PETBI_NEG_PAUSE | PETBI_NEG_DUPLEX,
			    PHYAddr[qdev->mac_index]);

	ql_mii_write_reg_ex(qdev, PETBI_CONTROL_REG,
			    PETBI_CTRL_AUTO_NEG | PETBI_CTRL_RESTART_NEG |
			    PETBI_CTRL_FULL_DUPLEX | PETBI_CTRL_SPEED_1000,
			    PHYAddr[qdev->mac_index]);
}