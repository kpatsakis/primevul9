static void ql_petbi_reset(struct ql3_adapter *qdev)
{
	ql_mii_write_reg(qdev, PETBI_CONTROL_REG, PETBI_CTRL_SOFT_RESET);
}