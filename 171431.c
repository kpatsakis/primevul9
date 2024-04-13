static void ql_petbi_start_neg(struct ql3_adapter *qdev)
{
	u16 reg;

	/* Enable Auto-negotiation sense */
	ql_mii_read_reg(qdev, PETBI_TBI_CTRL, &reg);
	reg |= PETBI_TBI_AUTO_SENSE;
	ql_mii_write_reg(qdev, PETBI_TBI_CTRL, reg);

	ql_mii_write_reg(qdev, PETBI_NEG_ADVER,
			 PETBI_NEG_PAUSE | PETBI_NEG_DUPLEX);

	ql_mii_write_reg(qdev, PETBI_CONTROL_REG,
			 PETBI_CTRL_AUTO_NEG | PETBI_CTRL_RESTART_NEG |
			 PETBI_CTRL_FULL_DUPLEX | PETBI_CTRL_SPEED_1000);

}