static void ql_phy_reset_ex(struct ql3_adapter *qdev)
{
	ql_mii_write_reg_ex(qdev, CONTROL_REG, PHY_CTRL_SOFT_RESET,
			    PHYAddr[qdev->mac_index]);
}