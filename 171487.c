static void ql_petbi_reset_ex(struct ql3_adapter *qdev)
{
	ql_mii_write_reg_ex(qdev, PETBI_CONTROL_REG, PETBI_CTRL_SOFT_RESET,
			    PHYAddr[qdev->mac_index]);
}