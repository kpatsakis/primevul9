static void ql_phy_init_ex(struct ql3_adapter *qdev)
{
	ql_phy_reset_ex(qdev);
	PHY_Setup(qdev);
	ql_phy_start_neg_ex(qdev);
}